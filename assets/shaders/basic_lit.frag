#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 color;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightIntensity;

void main()
{
    vec3 norm = normalize(Normal);

    // light comes from opposite direction
    vec3 lightDir = normalize(-lightDirection);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * lightColor * color * lightIntensity;

    FragColor = vec4(diffuse, 1.0);
}