#version 330 core
out vec4 FragColor;

in vec2 TextureCoord;
in vec3 Normal;
in vec3 FragPos;

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform Light light;
uniform vec3 viewPos;

uniform bool blinnSwitch;
uniform bool normalSwitch;

void main()
{
    vec3 normal;

    if(normalSwitch)
    {
        // 法线使用法线贴图中的数据，贴图的数据是rgb，范围为[0,1]，对应法线的xyz[-1,1]
        normal = texture(normalMap, TextureCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0); // [0, 1] -> [-1, 1]
    }
    else
    {
        normal = normalize(Normal);
    }

    vec3 color = texture(diffuseMap, TextureCoord).rgb;

    // ambient
    vec3 ambient = light.ambient;

    // diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(lightDir,normal), 0.0);
    vec3 diffuse = light.diffuse * diff * color;

    // specular
    vec3 viewDir = normalize(viewPos-FragPos);
    float spec = 0.0f;
    float shininess = 64.0f;

    if(blinnSwitch)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    }
    vec3 specular = light.specular * spec * color;

    vec3 lighting = ambient + diffuse + specular;
    FragColor = vec4(lighting, 1.0);
}