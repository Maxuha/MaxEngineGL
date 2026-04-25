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

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

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

    // diffuse
    vec3 diffuse = calculateDiffuse(light.intensity, light.color, lightDir, normal);

    //specular
    vec3 specular = calculateSpecular(light.intensity, light.color, lightDir, normal, viewDir);

    return diffuse + specular;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir) {
    if (light.range <= 0) {
        return vec3(0.0f);
    }

    vec3 lightDir = normalize(light.position - FragPos);

    // diffuse
    vec3 diffuse = calculateDiffuse(light.intensity, light.color, lightDir, normal);

    //specular
    vec3 specular = calculateSpecular(light.intensity, light.color, lightDir, normal, viewDir);

    //attenuation
    float attenuation = calculateAttenuation(light.constant, light.linear, light.quadratic, light.range);

    diffuse *= attenuation;
    specular *= attenuation;

    return diffuse + specular;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    vec3 result = vec3(0.0f);

    if (light.range <= 0.0f) {
        return result;
    }

    vec3 lightDir = normalize(light.position - FragPos);

    // diffuse
    vec3 diffuse = calculateDiffuse(light.intensity, light.color, lightDir, normal);

    //specular
    vec3 specular = calculateSpecular(light.intensity, light.color, lightDir, normal, viewDir);

    result = diffuse + specular;

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
