#version 460

layout(set = 2, binding = 3) uniform samplerCube skybox;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 CameraPosition;
layout (location = 2) in vec3 Normal;

layout (location = 0) out vec4 FragColor;

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

void main()
{
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(Position - CameraPosition);
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}