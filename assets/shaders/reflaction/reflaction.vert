#version 460

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    vec4 position;
} camera;

layout(set = 1, binding = 2) readonly buffer Model {
    mat4 position[];
} model;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout (location = 0) out vec3 Position;
layout (location = 1) out vec3 CameraPosition;
layout (location = 2) out vec3 Normal;

void main()
{
    mat4 pos = model.position[gl_BaseInstance];
    Normal = mat3(transpose(inverse(pos))) * aNormal;
    CameraPosition = vec3(camera.position);
    Position = vec3(pos * vec4(aPos, 1.0));
    gl_Position = camera.proj * camera.view * vec4(Position, 1.0);
}
