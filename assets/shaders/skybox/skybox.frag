#version 460

layout(set = 2, binding = 3) uniform samplerCube skybox;

layout (location = 0) in vec3 TexCoords;

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
    FragColor = texture(skybox, TexCoords);
//    FragColor = vec4(TexCoords, 1);
 //   FragColor = vec4(normalize(TexCoords) * 0.5 + 0.5, 1.0);
}