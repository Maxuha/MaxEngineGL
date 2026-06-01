#version 460
#extension GL_ARB_bindless_texture : require

layout(binding = 0) uniform sampler2D diffuse;
layout(binding = 1) uniform sampler2D specular;
layout(binding = 2) uniform sampler2D depthMap;

struct Material {
    vec4 shininess;
};

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

layout (std140, binding = 1) uniform LightsBlock {
    AmbientLight ambientLight;
    DirectionalLight directionalLight;
    PointLight pointLight[8];
    SpotLight spotLight[8];
} light;

layout (std140, binding = 3) uniform MaterialBlock {
    Material mat;
} material;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;
in vec3 CameraPos;

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    float shadow = 0.0;

    if(projCoords.z > 1.0 || projCoords.x > 1.0 || projCoords.x < 0.0 || projCoords.y > 1.0 || projCoords.y < 0.0)
    {
        return 1.0;
    }

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    vec2 texelSize = 1.0 / textureSize(depthMap, 0);

    for(int x = -4; x <= 4; ++x)
    {
        for(int y = -4; y <= 4; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 81.0;

    //shadow = currentDepth - 0.005 > closestDepth ? 1.0 : 0.0;

    return 1.0 - shadow;
}

vec3 calculateSpecular(float lightIntensity, vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir) {
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
  //  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.mat.shininess.a);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    return spec * lightColor * lightIntensity * vec3(texture(specular, TexCoord));
}

vec3 calculateDiffuse(float lightIntensity, vec3 lightColor, vec3 lightDir, vec3 normal) {
    float diff = max(dot(normal, lightDir), 0.0);
    return lightColor * lightIntensity * diff * vec3(texture(diffuse, TexCoord));
}

float calculateAttenuation(float distance, float range) {
    return pow(clamp(1.0 - (distance * distance) / (range * range), 0.0, 1.0), 2);
}

vec3 calculateAmbientLight(AmbientLight light) {
    return light.color * light.intensity * vec3(texture(diffuse, TexCoord));
}

vec3 calculateDirectionLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    vec4 pos = light.space * vec4(FragPos, 1.0);

    float shadow = ShadowCalculation(pos, normal, lightDir);

    vec3 diffuse = calculateDiffuse(light.intensity, light.color, lightDir, normal);

    vec3 specular = calculateSpecular(light.intensity, light.color, lightDir, normal, viewDir);

    return (diffuse + specular) * shadow;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir) {
    if (light.range <= 0) {
        return vec3(0.0f);
    }

    vec3 lightDir = normalize(light.position - FragPos);

    float distance = length(light.position - FragPos);

    float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);

    vec3 diffuse = calculateDiffuse(light.intensity, light.color, lightDir, normal);

    vec3 specular = calculateSpecular(light.intensity, light.color, lightDir, normal, viewDir);

    float attenuation = calculateAttenuation(distance, light.range);

    diffuse *= attenuation;
    specular *= attenuation;

    return (diffuse + specular) * shadow;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    vec3 result = vec3(0.0f);

    vec3 lightPosition = vec3(light.positionRange);

    float lightRange = light.positionRange.w;

    vec3 lightColor = vec3(light.colorIntensity);

    vec3 lightDirection = vec3(light.direction);

    float lightIntensity = light.colorIntensity.w;

    float lightInnerCutOff = light.coneAngle.x;

    float lightOuterCutOff = light.coneAngle.y;

    if (lightRange <= 0.0f) {
        return result;
    }

    vec3 lightDir = normalize(lightPosition - FragPos);

    float distance = length(lightPosition - FragPos);

    float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);

    vec3 diffuse = calculateDiffuse(lightIntensity, lightColor, lightDir, normal);

    vec3 specular = calculateSpecular(lightIntensity, lightColor, lightDir, normal, viewDir);

    result = (diffuse + specular) * shadow;

    float theta = dot(normalize(lightDirection), -lightDir);
    float epsillon = lightInnerCutOff - lightOuterCutOff;
    float intensity = clamp((theta - lightOuterCutOff) / epsillon, 0.0, 1.0);
    result *= intensity;

    //attenuation
    float attenuation = calculateAttenuation(distance, lightRange);
    result *= attenuation;

    return result;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(CameraPos - FragPos);

    vec3 result = calculateAmbientLight(light.ambientLight);

    result += calculateDirectionLight(light.directionalLight, norm, viewDir);

    for (int i = 0; i < light.pointLight.length(); i++) {
        result += calculatePointLight(light.pointLight[i], norm, viewDir);
    }

    for (int i = 0; i < light.spotLight.length(); i++) {
        result += calculateSpotLight(light.spotLight[i], norm, viewDir);
    }

//    vec3 result = calculateDirectionLight(light.directionalLight, norm, viewDir);

    FragColor = vec4(result, 1.0);
}
