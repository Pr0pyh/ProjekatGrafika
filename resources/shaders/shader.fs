#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    sampler2D texture_opacity1;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPos;
in mat3 tbnMatrix;

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform vec3 viewPos;
uniform vec3 cameraPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 textureCoords)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, textureCoords));
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 textureCoords)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*(distance*distance));

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, textureCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 textureCoords)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*(distance*distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta-light.outerCutOff)/epsilon, 0.0f, 1.0f);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.texture_diffuse1, textureCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, textureCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return ambient + diffuse + specular;
}

vec2 ParallaxMapping(vec2 textureCoords, vec3 viewDirection)
{
    float height = texture(material.texture_height1, textureCoords).r;
    vec2 p = viewDirection.xy * (height*0.1);
    return textureCoords-p;
}

void main()
{
    vec3 parallaxView = normalize(tbnMatrix*viewPos - tbnMatrix*FragPos);
    vec2 parallaxCoords = ParallaxMapping(TexCoords, parallaxView);
    vec3 snapshotNormal = normalize(Normal);
    vec3 norm = texture(material.texture_normal1, parallaxCoords).rgb;
    norm = normalize(norm*2.0-1.0);
    norm = normalize(tbnMatrix*norm);

    vec3 viewDir = normalize(viewPos - FragPos);
  	vec3 cameraDir = normalize(-cameraPos);
  	if(dot(snapshotNormal, cameraDir) < 0.0)
  	    discard;

    vec3 result = CalcDirLight(dirLight, norm, viewDir, TexCoords);
    for(int i = 0; i<2; i++)
    {
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir, TexCoords);
    }
    result += calcSpotLight(spotLight, norm, FragPos, viewDir, TexCoords);

    vec3 opacityTexture = vec3(texture(material.texture_opacity1, TexCoords));
    float opacityFactor = (opacityTexture.r + opacityTexture.g + opacityTexture.b)*0.3333;
    FragColor = vec4(result, opacityFactor);
}