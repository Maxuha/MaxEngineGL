#version 420

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform vec3 color;

uniform sampler2D ourTexture;
uniform sampler2D depthMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(depthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth - 0.005 > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    float depthValue = texture(depthMap, TexCoord).r;

    float shadow = ShadowCalculation(FragPosLightSpace);
    FragColor = texture(ourTexture, TexCoord) * (1.0 - shadow);
  //  FragColor = texture(vec3(depthValue), TexCoord) ;
}