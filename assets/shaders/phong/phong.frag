#version 460


//layout(set = 2, binding = 4) uniform sampler2DShadow depthMap;

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

layout (set = 0, binding = 1) uniform LightsBlock {
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

layout (location = 0) in vec3 FragPos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in vec3 CameraPos;
layout (location = 4) in vec4 FragPosLightSpace;

layout (location = 0) out vec4 FragColor;

//float DirShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
//{
//    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
//
//    vec4 shadowCoords = fragPosLightSpace;
//
//    shadowCoords.xy = (shadowCoords.xy * 0.5) + (vec2(0.5) * shadowCoords.w);
//
//    shadowCoords.z = (shadowCoords.z * 0.5) + (0.5 * shadowCoords.w);
//
//    shadowCoords.z -= bias * shadowCoords.w;
//
//    return textureProj(depthMap, shadowCoords);
//}
//
//vec3 calculateSpecular(float lightIntensity, vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir, vec3 specTex) {
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    return spec * lightColor * lightIntensity * specTex;
//}

vec3 calculateSpecular(float lightIntensity, vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir, vec3 specTex) {
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    return spec * lightColor * lightIntensity * specTex;
}

vec3 calculateDiffuse(float lightIntensity, vec3 lightColor, vec3 lightDir, vec3 normal, vec3 diffTex) {
    float diff = max(dot(normal, lightDir), 0.0);
    return lightColor * lightIntensity * diff  * diffTex;
   // return lightColor * lightIntensity * diff * vec3(material.diffuseColor) * diffTex;
}
//
//float calculateAttenuation(float distance, float range) {
//    return pow(clamp(1.0 - (distance * distance) / (range * range), 0.0, 1.0), 2);
//}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(CameraPos - FragPos);

    vec3 diffTex = vec3(texture(diffuse, TexCoord));
    vec3 specTex = vec3(texture(specular, TexCoord));

  //  specTex += 0.1;

    vec3 result = light.ambientLight.color * light.ambientLight.intensity * diffTex;

//    vec3 result = light.ambientLight.color * light.ambientLight.intensity * vec3(material.diffuseColor) * diffTex;
//
    vec3 dirLightDir = normalize(-light.directionalLight.direction);
//    float dirShadow = DirShadowCalculation(FragPosLightSpace, norm, dirLightDir);
//
    vec3 dirDiffuse = calculateDiffuse(light.directionalLight.intensity, light.directionalLight.color, dirLightDir, norm, diffTex);
    result += dirDiffuse;
    vec3 dirSpecular = calculateSpecular(light.directionalLight.intensity, light.directionalLight.color, dirLightDir, norm, viewDir, specTex);
    result += dirSpecular;
//    vec3 dirSpecular = calculateSpecular(light.directionalLight.intensity, light.directionalLight.color, dirLightDir, norm, viewDir, specTex);
//    result += (dirDiffuse + dirSpecular) * dirShadow;
//
//    for (int i = 0; i < 8; i++) {
//        if (light.pointLight[i].range <= 0.0) continue;
//
//        vec3 lightDir = light.pointLight[i].position - FragPos;
//        float distance = length(lightDir);
//        lightDir = normalize(lightDir);
//
//        float attenuation = calculateAttenuation(distance, light.pointLight[i].range);
//        if (attenuation <= 0.0) continue;
//
//        vec3 diff = calculateDiffuse(light.pointLight[i].intensity, light.pointLight[i].color, lightDir, norm, diffTex);
//        vec3 spec = calculateSpecular(light.pointLight[i].intensity, light.pointLight[i].color, lightDir, norm, viewDir, specTex);
//
//        result += (diff + spec) * attenuation;
//    }
//
//    for (int i = 0; i < 8; i++) {
//        float lightRange = light.spotLight[i].positionRange.w;
//        if (lightRange <= 0.0) continue;
//
//        vec3 lightPosition = light.spotLight[i].positionRange.xyz;
//        vec3 lightDir = lightPosition - FragPos;
//        float distance = length(lightDir);
//        lightDir = normalize(lightDir);
//
//        float attenuation = calculateAttenuation(distance, lightRange);
//        if (attenuation <= 0.0) continue;
//
//        vec3 lightDirection = normalize(light.spotLight[i].direction.xyz);
//        float theta = dot(lightDirection, -lightDir);
//        float epsilon = light.spotLight[i].coneAngle.x - light.spotLight[i].coneAngle.y;
//        float intensity = clamp((theta - light.spotLight[i].coneAngle.y) / epsilon, 0.0, 1.0);
//
//        if (intensity <= 0.0) continue;
//
//        vec3 lightColor = light.spotLight[i].colorIntensity.xyz;
//        float lightIntensity = light.spotLight[i].colorIntensity.w;
//
//        vec3 diff = calculateDiffuse(lightIntensity, lightColor, lightDir, norm, diffTex);
//        vec3 spec = calculateSpecular(lightIntensity, lightColor, lightDir, norm, viewDir, specTex);
//
//        result += (diff + spec) * (attenuation * intensity);
//    }

//    FragColor = vec4(result, 1.0);
    FragColor = vec4(result, 1.0);
}
