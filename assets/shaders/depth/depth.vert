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

layout (location = 0) in vec3 aPos;

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    vec4 position;
} camera;

layout (set = 0, binding = 1) uniform LightsBlock {
    AmbientLight ambientLight;
    DirectionalLight directionalLight;
    PointLight pointLight[8];
    SpotLight spotLight[8];
} light;

layout(set = 1, binding = 2) uniform Model {
    mat4 position;
} model;

layout (set = 2, binding = 3) uniform MaterialBlock {
    float shininess;
    vec4 diffuseColor;
} material;

layout(set = 2, binding = 4) uniform sampler2D diffuse;
layout(set = 2, binding = 5) uniform sampler2D specular;
layout(set = 2, binding = 6) uniform sampler2D depthMap;

vec2 positions2[3] = vec2[](
        vec2(0.0, -0.5),
        vec2(0.5, 0.5),
        vec2(-0.5, 0.5)
);

vec4 positions[3] = vec4[](
        vec4(0.0, -0.5, 0.0, 1.0),
        vec4(0.5, 0.5, 0.0, 1.0),
        vec4(-0.5, 0.5, 0.0, 1.0)
);


void main() {
//    gl_Position = camera.proj * camera.view * model.position * vec4(aPos, 1);
//    if (gl_VertexIndex % 2 == 0) {
//        index = 1.0;
//    } else {
//        index = 0.0;
//    }
//
//    gl_Position = camera.proj * camera.view * model.position * vec4(positions[gl_VertexIndex], index, 1.0);
//    gl_Position = camera.proj * camera.view * model.position * vec4(positions[gl_VertexIndex], 1.0);
//    vec4 current_pos = positions[gl_VertexIndex];
//    gl_Position =  current_pos;
  //  gl_Position = vec4(positions2[gl_VertexIndex], 0.5, 1.0);

    vec4 current_pos;

    // ?????????? ?????? ???, ??? ??? ??????? 0, 1, 2 ? ?????????
    switch(gl_VertexIndex) {
        // --- ???????? ????? (Z = 2.5) ---
        case 0:  current_pos = vec4(-2.5, -2.5,  2.5, 1.0); break;
        case 1:  current_pos = vec4( 2.5, -2.5,  2.5, 1.0); break;
        case 2:  current_pos = vec4( 2.5,  2.5,  2.5, 1.0); break;
        case 3:  current_pos = vec4( 2.5,  2.5,  2.5, 1.0); break;
        case 4:  current_pos = vec4(-2.5,  2.5,  2.5, 1.0); break;
        case 5:  current_pos = vec4(-2.5, -2.5,  2.5, 1.0); break;

        // --- ?????? ????? (Z = -2.5) ---
        case 6:  current_pos = vec4(-2.5, -2.5, -2.5, 1.0); break;
        case 7:  current_pos = vec4(-2.5,  2.5, -2.5, 1.0); break;
        case 8:  current_pos = vec4( 2.5,  2.5, -2.5, 1.0); break;
        case 9:  current_pos = vec4( 2.5,  2.5, -2.5, 1.0); break;
        case 10: current_pos = vec4( 2.5, -2.5, -2.5, 1.0); break;
        case 11: current_pos = vec4(-2.5, -2.5, -2.5, 1.0); break;

        // --- ????? ????? (X = -2.5) ---
        case 12: current_pos = vec4(-2.5,  2.5,  2.5, 1.0); break;
        case 13: current_pos = vec4(-2.5,  2.5, -2.5, 1.0); break;
        case 14: current_pos = vec4(-2.5, -2.5, -2.5, 1.0); break;
        case 15: current_pos = vec4(-2.5, -2.5, -2.5, 1.0); break;
        case 16: current_pos = vec4(-2.5, -2.5,  2.5, 1.0); break;
        case 17: current_pos = vec4(-2.5,  2.5,  2.5, 1.0); break;

        // --- ?????? ????? (X = 2.5) ---
        case 18: current_pos = vec4( 2.5,  2.5,  2.5, 1.0); break;
        case 19: current_pos = vec4( 2.5, -2.5,  2.5, 1.0); break;
        case 20: current_pos = vec4( 2.5, -2.5, -2.5, 1.0); break;
        case 21: current_pos = vec4( 2.5, -2.5, -2.5, 1.0); break;
        case 22: current_pos = vec4( 2.5,  2.5, -2.5, 1.0); break;
        case 23: current_pos = vec4( 2.5,  2.5,  2.5, 1.0); break;

        // --- ??????? ????? (Y = 2.5) ---
        case 24: current_pos = vec4(-2.5,  2.5, -2.5, 1.0); break;
        case 25: current_pos = vec4(-2.5,  2.5,  2.5, 1.0); break;
        case 26: current_pos = vec4( 2.5,  2.5,  2.5, 1.0); break;
        case 27: current_pos = vec4( 2.5,  2.5,  2.5, 1.0); break;
        case 28: current_pos = vec4( 2.5,  2.5, -2.5, 1.0); break;
        case 29: current_pos = vec4(-2.5,  2.5, -2.5, 1.0); break;

        // --- ?????? ????? (Y = -2.5) ---
        case 30: current_pos = vec4(-2.5, -2.5, -2.5, 1.0); break;
        case 31: current_pos = vec4( 2.5, -2.5, -2.5, 1.0); break;
        case 32: current_pos = vec4( 2.5, -2.5,  2.5, 1.0); break;
        case 33: current_pos = vec4( 2.5, -2.5,  2.5, 1.0); break;
        case 34: current_pos = vec4(-2.5, -2.5,  2.5, 1.0); break;
        case 35: current_pos = vec4(-2.5, -2.5, -2.5, 1.0); break;

        default: current_pos = vec4(0.0, 0.0, 0.0, 1.0);   break;
    }


    mat4 m = mat4 (1) ;

   // gl_Position = camera.proj * camera.view * m * current_pos;
    gl_Position = camera.proj * camera.view * m * vec4(aPos, 1);
}
