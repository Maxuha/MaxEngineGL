#version 460

struct AmbientLight {
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    mat4 space;
    vec3 color;
    float intensity;
    vec3 direction;
    float padding;
};

struct PointLight {
    vec3 color;
    float intensity;
    vec3 position;
    float range;
};

struct SpotLight {
    vec4 colorIntensity;
    vec4 positionRange;
    vec4 direction;
    vec4 coneAngle;
};

layout (set = 0, binding = 1, std140) uniform LightsBlock {
    AmbientLight ambientLight;
    DirectionalLight directionalLight;
    PointLight pointLight[8];
    SpotLight spotLight[8];
} light;

layout (set = 2, binding = 3) uniform MaterialBlock {
    float shininess;
    vec4 diffuseColor;
} material;

layout(set = 2, binding = 4) uniform sampler2D diffuse;
layout(set = 2, binding = 5) uniform sampler2D specular;
layout(set = 2, binding = 6) uniform sampler2D depthMap;
layout(set = 2, binding = 7) uniform sampler2D normalMap;
layout(set = 2, binding = 8) uniform sampler2D heightMap;

layout (location = 0) in vec3 FragPos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in vec3 CameraPos;
layout (location = 4) in vec4 FragPosLightSpace;
layout (location = 5) in mat3 TBN;

layout (location = 0) out vec4 FragColor;

// TODO pass from engine
float height_scale = 0.015;
float biasDepth = 0.05;

float dirShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    if (fragPosLightSpace.w <= 0.0) {
        return 1.0;
    }

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.z < 0.0 ||
        projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 1.0;
    }

    float closestDepth = texture(depthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(biasDepth * (1.0 - dot(normal, lightDir)), 0.00005);
    float shadow = (currentDepth - bias) > closestDepth ? 0.0 : 1.0;

    return shadow;
}

vec3 calculateSpecular(float lightIntensity, vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir, vec3 specTex) {
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    return spec * lightColor * lightIntensity * specTex;
}

vec3 calculateDiffuse(float lightIntensity, vec3 lightColor, vec3 lightDir, vec3 normal, vec3 diffTex) {
    float diff = max(dot(normal, lightDir), 0.0);
    return lightColor * lightIntensity * diff  * diffTex;
}

float calculateAttenuation(float distance, float range) {
    return pow(clamp(1.0 - (distance * distance) / (range * range), 0.0, 1.0), 2);
}

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir) {
    // Determine the range for dynamic layer counts based on the viewing angle
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

    // Calculate the depth step size for each individual layer
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    // Protect against division by zero at extreme grazing angles (viewDir.z close to 0)
    float viewZ = max(abs(viewDir.z), 0.00001);

    // Calculate the maximum UV shift vector based on the height scale property
    vec2 P = (viewDir.xy / viewZ) * height_scale;

    // Determine the UV offset step size per layer iteration
    vec2 deltaTexCoords = P / numLayers;

    // Initialize the tracking texture coordinates
    vec2 currentTexCoords = texCoords;

    // Read the current heightmap depth value (0.0 represents the top surface, 1.0 represents the deepest cavity)
    float currentDepthMapValue = texture(heightMap, currentTexCoords).r;

    // Raymarching loop: step down through depth layers until a collision with the heightfield is detected
    for(int i = 0; i < int(maxLayers); i++) {
        if (currentLayerDepth >= currentDepthMapValue) {
            break; // Ray has intersected or gone below the heightmap surface
        }

        // Move to the next depth slice layer (subtracting UV to shift coordinates along the view direction)
        currentTexCoords -= deltaTexCoords;
        // Sample the depth value at the new shifted texture coordinates
        currentDepthMapValue = texture(heightMap, currentTexCoords).r;
        // Increase the current virtual depth of the ray
        currentLayerDepth += layerDepth;
    }

    // Get the texture coordinates of the previous step (just before the intersection)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // Calculate the depth differences before and after the intersection point for interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (texture(heightMap, prevTexCoords).r) - (currentLayerDepth - layerDepth);

    // Linear interpolation between the two closest samples to smooth out stepping artifacts (POM)
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = mix(currentTexCoords, prevTexCoords, weight);

    return finalTexCoords;
}

void main()
{
    vec3 viewDir = normalize(CameraPos - FragPos);
    vec3 viewDirTangentSpace = normalize(transpose(TBN) * viewDir);
    vec2 texCoords = parallaxMapping(TexCoord, viewDirTangentSpace);

    vec3 diffTex = texture(diffuse, texCoords).rgb;
    vec3 specTex = texture(specular, texCoords).rgb;
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(TBN * normal);

    //Ambient
    vec3 result = light.ambientLight.color * light.ambientLight.intensity * diffTex;

    //Directional
    vec3 dirLightDir = normalize(-light.directionalLight.direction);
    vec3 dirDiffuse = calculateDiffuse(light.directionalLight.intensity, light.directionalLight.color, dirLightDir, normal, diffTex);
    vec3 dirSpecular = calculateSpecular(light.directionalLight.intensity, light.directionalLight.color, dirLightDir, normal, viewDir, specTex);
    float dirShadow = dirShadowCalculation(FragPosLightSpace, normal, dirLightDir);

    result += (dirDiffuse + dirSpecular) * (dirShadow);

    // Point
    for (int i = 0; i < 8; i++) {
        if (light.pointLight[i].range <= 0.0) continue;

        vec3 lightDir = light.pointLight[i].position - FragPos;
        float distance = length(lightDir);
        lightDir = normalize(lightDir);

        float attenuation = calculateAttenuation(distance, light.pointLight[i].range);
        if (attenuation <= 0.0) continue;

        vec3 diff = calculateDiffuse(light.pointLight[i].intensity, light.pointLight[i].color, lightDir, normal, diffTex);
        vec3 spec = calculateSpecular(light.pointLight[i].intensity, light.pointLight[i].color, lightDir, normal, viewDir, specTex);

        result += (diff + spec) * attenuation;
    }

    // Spot
    for (int i = 0; i < 8; i++) {
        float lightRange = light.spotLight[i].positionRange.w;
        if (lightRange <= 0.0) continue;

        vec3 lightPosition = light.spotLight[i].positionRange.xyz;
        vec3 lightDir = lightPosition - FragPos;
        float distance = length(lightDir);
        lightDir = normalize(lightDir);

        float attenuation = calculateAttenuation(distance, lightRange);
        if (attenuation <= 0.0) continue;

        vec3 lightDirection = normalize(light.spotLight[i].direction.xyz);
        float theta = dot(lightDirection, -lightDir);
        float epsilon = light.spotLight[i].coneAngle.x - light.spotLight[i].coneAngle.y;
        float intensity = clamp((theta - light.spotLight[i].coneAngle.y) / epsilon, 0.0, 1.0);

        if (intensity <= 0.0) continue;

        vec3 lightColor = light.spotLight[i].colorIntensity.xyz;
        float lightIntensity = light.spotLight[i].colorIntensity.w;

        vec3 diff = calculateDiffuse(lightIntensity, lightColor, lightDir, normal, diffTex);
        vec3 spec = calculateSpecular(lightIntensity, lightColor, lightDir, normal, viewDir, specTex);

        result += (diff + spec) * (attenuation * intensity);
    }

    FragColor = vec4(result, 1.0);
}
