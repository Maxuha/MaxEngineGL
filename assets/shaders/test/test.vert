#version 460
#extension GL_GOOGLE_include_directive : require

#include "camera.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

void main() {
    gl_Position = camera.proj * camera.view * model.model * vec4(inPosition, 1.0);
    outTexCoord = inTexCoord;
}
