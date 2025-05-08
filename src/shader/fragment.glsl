#version 430 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    // attenuation
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 pos;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    // spot size
    float cutOff;
    float outerCutOff;
};


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform PointLight light;

void main()
{
    vec3 lightDir = normalize(light.pos - FragPos);

    // point
    float distance = length(light.pos - FragPos);
    float attenuation = 1.0 / (light.quadratic + light.linear * distance + light.quadratic * (distance * distance));
    // spot
    // float theta = dot(lightDir, normalize(-light.direction));
    // float epsilon = light.cutOff - light.outerCutOff;
    // float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 result = vec3(0.0, 0.0, 0.0);
    // if (theta > light.cutOff) {
    // ambient
        vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

        // diffuse
        vec3 norm = normalize(Normal);
        // point
        // vec3 lightDir = normalize(-light.direction); // direction
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

        //specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        result = ambient + diffuse + specular;
    // }
    // else {
    //     result = vec3(light.ambient * vec3(texture(material.diffuse, TexCoords)));
    // }
    FragColor = vec4(result, 1.0);
    // FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}
