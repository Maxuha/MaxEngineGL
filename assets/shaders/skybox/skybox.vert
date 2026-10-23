#version 460

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    vec4 position;
} camera;

layout (location = 0) in vec3 aPos;

layout (location = 0) out vec3 TexCoords;

void main()
{
    TexCoords = aPos;
    mat4 staticView = mat4(mat3(camera.view));
    vec4 pos = camera.proj * staticView * vec4(aPos, 1.0);
    gl_Position = pos.xyww;

}
