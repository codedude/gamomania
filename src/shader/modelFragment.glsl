#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float texture_shininess;
};

uniform Material material;

void main()
{
    FragColor = texture(material.texture_diffuse1, TexCoords);
}
