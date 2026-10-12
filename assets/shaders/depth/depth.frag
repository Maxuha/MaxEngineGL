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

void main()
{
}
