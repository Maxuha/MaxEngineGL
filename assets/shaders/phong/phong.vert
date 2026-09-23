#version 460

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    vec4 position;
} camera;

//layout(set = 0, binding = 7) uniform LightSpaceMatrix {
//    mat4 space;
//} lightSpaceMatrix;

layout(set = 1, binding = 2) uniform Model {
    mat4 position;
} model;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 TexCoord;
layout (location = 3) out vec3 CameraPos;
layout (location = 4) out vec4 FragPosLightSpace;

mat4 m = mat4(
        1.73205, 0.0, 0.0, 0.0, // 1-? ???????
        0.0, 0.0, -1.0, -1.0, // 2-? ???????
        0.0, 1.73205, 0.0, 0.0, // 3-? ???????
        0.0, 0.0, 4.999, 5.0  // 4-? ???????
);


//mat4 m = mat4(
//        1.0, 0.0, 0.0, 0.0, // 1-? ???????
//        0.0, 0.0, 1.0, 0.0, // 2-? ???????
//        0.0, -1.0, 0.0, 9.9901, // 3-? ???????
//        0.0, -1.0, 0.0, 1.0  // 4-? ???????
//);

void main()
{
    FragPos = vec3(model.position * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model.position))) * aNormal;
    TexCoord = aTexCoord;
    CameraPos = vec3(camera.position);
//    FragPosLightSpace = lightSpaceMatrix.space * vec4(FragPos, 1.0);
    FragPosLightSpace = m * vec4(FragPos, 1.0);
    gl_Position = camera.proj * camera.view * vec4(FragPos, 1.0);
}
