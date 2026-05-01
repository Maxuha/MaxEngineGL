#version 420

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct AmbientLight {
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    vec3 color;
    vec3 direction;
    float intensity;
};

struct PointLight {
    vec3 color;
    vec3 position;
    float intensity;
    float range;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 color;
    vec3 position;
    vec3 direction;
    float intensity;
    float range;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

uniform vec3 viewPos;

uniform Material material;

uniform AmbientLight ambientLight;

uniform DirectionalLight directionalLight;

#define NR_POINT_LIGHTS 64
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform SpotLight spotLight;

uniform sampler2D depthMap;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    float shadow = 0.0;

    if(projCoords.z > 1.0)
    {
        return shadow;
    }

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    vec2 texelSize = 1.0 / textureSize(depthMap, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    return 1 - shadow;
}

vec3 calculateSpecular(float lightIntensity, vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir) {
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    return spec * lightColor * lightIntensity * vec3(texture(material.specular, TexCoord));
}

vec3 calculateDiffuse(float lightIntensity, vec3 lightColor, vec3 lightDir, vec3 normal) {
    float diff = max(dot(normal, lightDir), 0.0);
    return lightColor * lightIntensity * diff * vec3(texture(material.diffuse, TexCoord));
}

float calculateAttenuation(float constant, float linear, float quadratic, float distance) {
    return 1 / (constant + linear * distance + quadratic * (distance * distance));
}

vec3 calculateAmbientLight(AmbientLight light) {
    return light.color * light.intensity * vec3(texture(material.diffuse, TexCoord));
}

vec3 calculateDirectionLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);

    // diffuse
    vec3 diffuse = calculateDiffuse(light.intensity, light.color, lightDir, normal);

    //specular
    vec3 specular = calculateSpecular(light.intensity, light.color, lightDir, normal, viewDir);

    return (diffuse + specular) * shadow;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir) {
    if (light.range <= 0) {
        return vec3(0.0f);
    }

    vec3 lightDir = normalize(light.position - FragPos);

    float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);

    // diffuse
    vec3 diffuse = calculateDiffuse(light.intensity, light.color, lightDir, normal);

    //specular
    vec3 specular = calculateSpecular(light.intensity, light.color, lightDir, normal, viewDir);

    //attenuation
    float attenuation = calculateAttenuation(light.constant, light.linear, light.quadratic, light.range);

    diffuse *= attenuation;
    specular *= attenuation;

    return (diffuse + specular) * shadow;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    vec3 result = vec3(0.0f);

    if (light.range <= 0.0f) {
        return result;
    }

    vec3 lightDir = normalize(light.position - FragPos);

    float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);

    // diffuse
    vec3 diffuse = calculateDiffuse(light.intensity, light.color, lightDir, normal);

    //specular
    vec3 specular = calculateSpecular(light.intensity, light.color, lightDir, normal, viewDir);

    result = (diffuse + specular) * shadow;

    float theta = dot(normalize(light.direction), -lightDir);
    float epsillon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsillon, 0.0, 1.0);
    result *= intensity;

    //attenuation
    float attenuation = calculateAttenuation(light.constant, light.linear, light.quadratic, light.range);
    result *= attenuation;

    return result;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 light = calculateAmbientLight(ambientLight);

    light += calculateDirectionLight(directionalLight, norm, viewDir);

    for (int i = 0; i < pointLights.length(); i++) {
        light += calculatePointLight(pointLights[i], norm, viewDir);
    }

    light += calculateSpotLight(spotLight, norm, viewDir);

    FragColor = vec4(light, 1.0);
}
