#version 460

layout (location = 0) in vec3 aPos;

layout (location = 3) uniform mat4 model;
layout (location = 4) uniform mat4 space;

void main() {
    gl_Position = space * model * vec4(aPos, 1);
}
