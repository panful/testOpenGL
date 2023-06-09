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
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

uniform Light light;
uniform vec3 viewPos;

void main()
{
    // 从G缓冲中获取数据
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
    float AmbientOcclusion = texture(ssao, TexCoords).r;

    vec3 viewDir = normalize(viewPos - FragPos);

    // 环境光
    vec3 lighting = Albedo * 0.1 * AmbientOcclusion;

    // 漫反射
    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * light.Color;
    
    // 镜面反射
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, Normal), 0.0), 8);
    vec3 specular = light.Color * spec;

    // 光照衰减
    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);

    lighting += (diffuse + specular) * attenuation;
    
    FragColor = vec4(lighting, 1.0);
}