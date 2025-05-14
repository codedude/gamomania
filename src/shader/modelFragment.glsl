#version 430 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
    sampler2D texDiffuse;
    sampler2D texSpecular;
    sampler2D texBump;
    sampler2D texHeight;
    vec3 color;
    vec3 specular;
    float shininess;
};

struct DirectionalLight {
    vec3 color;
    vec3 dir;
    float intensity;
};

struct PointLight {
    vec3 pos;
    vec3 color;
    float intensity;
    // attenuation
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 pos;
    vec3 dir;
    vec3 color;
    float intensity;
    // halo
    float cutOff;
    float outerCutOff;
    // attenuation
    float constant;
    float linear;
    float quadratic;
};

struct AmbientLight {
    vec3 color;
    float intensity;
};

#define N_POINT_LIGHTS 16
#define N_DIRECTIONAL_LIGHTS 4
#define N_SPOT_LIGHTS 8
#define N_AMBIENT_LIGHTS 4

uniform vec3 viewPos;
uniform int pointLightsLen;
uniform int directionalLightsLen;
uniform int spotLightsLen;
uniform int ambientLightsLen;
uniform Material material;
uniform PointLight pointLights[N_POINT_LIGHTS];
uniform DirectionalLight directionalLights[N_DIRECTIONAL_LIGHTS];
uniform SpotLight spotLights[N_SPOT_LIGHTS];
uniform AmbientLight ambientLights[N_AMBIENT_LIGHTS];

// function prototypes
vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcAmbientLight(AmbientLight light);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0, 0.0, 0.0);

    for (int i=0; i< ambientLightsLen; ++i) {
        result += calcAmbientLight(ambientLights[i]);
    }
    for (int i=0; i< directionalLightsLen; ++i) {
        result += calcDirLight(directionalLights[i], norm, viewDir);
    }
    for (int i=0; i< pointLightsLen; ++i) {
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    for (int i=0; i< spotLightsLen; ++i) {
        result += calcSpotLight(spotLights[i], norm, FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirectionalLight light,  vec3 normal, vec3 viewDir) {
    vec3 color = light.color * light.intensity;
    vec3 lightDir = normalize(-light.dir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 diffuse = color * diff * vec3(texture(material.texDiffuse, TexCoords)) * material.color;
    vec3 specular = color * spec * vec3(texture(material.texSpecular, TexCoords)) * material.specular;
    return (diffuse + specular);
}

vec3 calcPointLight(PointLight light,  vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 color = light.color * light.intensity;
    vec3 lightDir = normalize(light.pos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 diffuse = color * diff * vec3(texture(material.texDiffuse, TexCoords)) * material.color;
    vec3 specular = color * spec * vec3(texture(material.texSpecular, TexCoords)) * material.specular;
    diffuse *= attenuation;
    specular *= attenuation;
    return (diffuse + specular);
}

vec3 calcSpotLight(SpotLight light,  vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 color = light.color * light.intensity;
    vec3 lightDir = normalize(light.pos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.dir));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 diffuse = color * diff * vec3(material.color);
    vec3 specular = color * spec * vec3(material.specular);
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (diffuse + specular);
}

vec3 calcAmbientLight(AmbientLight light) {
    vec3 color = light.color * light.intensity;
    return color;
}
