#version 420

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;

uniform Light light;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 LightPos;

out vec4 FragColor;

uniform vec3 color;
uniform sampler2D ourTexture;

void main()
{
    // ambient
    vec3 ambient = material.ambient * light.ambient;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff * light.diffuse;

    //specular
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(-FragPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

//    vec4 result = vec4((ambient + diffuse + specular), 1) * texture(ourTexture, TexCoord) * vec4(color, 1);
    vec3 result = (ambient + diffuse + specular) * color;

    FragColor = vec4(result, 1.0);
}
