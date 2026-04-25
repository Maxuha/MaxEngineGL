#version 420

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 LightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightDirection;

void main()
{
    vec3 worldFragPos = vec3(model * vec4(aPos, 1.0));
    FragPos = vec3(view * vec4(worldFragPos, 1));
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    LightPos = vec3(view * vec4(lightDirection, 1.0));
    TexCoord = aTexCoord;
    gl_Position = projection * vec4(FragPos, 1.0);
}
