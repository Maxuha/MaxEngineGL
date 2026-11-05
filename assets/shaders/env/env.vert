#version 460

#extension GL_ARB_shader_viewport_layer_array : require

layout(set = 2, binding = 3) uniform Camera {
    mat4 proj;
    mat4 view[6];
} camera;

layout (location = 0) in vec3 aPos;
layout (location = 5) in int in_InstanceID;

layout (location = 0) out vec3 FragPos;

void main()
{
    gl_Layer = in_InstanceID;

    FragPos = aPos;
    gl_Position = camera.proj * camera.view[in_InstanceID]  * vec4(FragPos, 1.0);

}
