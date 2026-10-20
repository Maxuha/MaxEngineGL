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

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    vec4 position;
} camera;


layout (set = 0, binding = 1, std140) uniform LightsBlock {
    AmbientLight ambientLight;
    DirectionalLight directionalLight;
    PointLight pointLight[8];
    SpotLight spotLight[8];
} light;

layout(set = 1, binding = 2) readonly buffer Model {
    mat4 position[];
} model;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 TexCoord;
layout (location = 3) out vec3 CameraPos;
layout (location = 4) out vec4 FragPosLightSpace;
layout (location = 5) out mat3 TBN;

void main()
{
    mat4 modelMatrix = model.position[gl_BaseInstance];
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;
    TexCoord = aTexCoord;
    CameraPos = vec3(camera.position);
    FragPosLightSpace = light.directionalLight.space * vec4(FragPos, 1.0);

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(Normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

    gl_Position =  camera.proj * camera.view  * vec4(FragPos, 1.0);
}
