#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

struct Light{
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

const int NUM_OF_LIGHTS = 8;

uniform Light lights[NUM_OF_LIGHTS];
uniform vec3 viewPos;

void main()
{
    // 从G缓冲中获取数据
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    // 环境光
    vec3 lighting = Albedo * 0.1;

    vec3 viewDir = normalize(viewPos - FragPos);
    for(int i = 0; i < NUM_OF_LIGHTS; ++i)
    {
        // 漫反射
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * lights[i].Color;
        
        // 镜面反射
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfwayDir, Normal), 0.0), 32);
        vec3 specular = lights[i].Color * spec * Specular;

        // 光照衰减
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);

        lighting += (diffuse + specular) * attenuation;
    }

    FragColor = vec4(lighting, 1.0);
}