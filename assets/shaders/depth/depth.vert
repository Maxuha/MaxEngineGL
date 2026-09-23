#version 460

layout (location = 0) in vec3 aPos;

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    vec4 position;
} camera;

layout(set = 1, binding = 2) uniform Model {
    mat4 position;
} model;

void main() {
    gl_Position = camera.proj * camera.view * model.position * vec4(aPos, 1);
}
