#version 420

layout (location = 0) in vec3 aPos;

//uniform mat4 view;
//uniform mat4 projection;
uniform mat4 space;
uniform mat4 model;

void main() {
    gl_Position = space * model * vec4(aPos, 1);
}
