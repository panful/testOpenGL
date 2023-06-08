#version 330 core

layout (location = 0) out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light{
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};

uniform sampler2D difuseTex;
uniform vec3 viewPos;
uniform float specular;

const int NUM_OF_LIGHTS = 8;
uniform Light lights[NUM_OF_LIGHTS];

void main()
{
    vec3 FragPos = fs_in.FragPos;
    vec3 Normal = fs_in.Normal;
    vec3 Albedo = texture(difuseTex, fs_in.TexCoords).rgb;

    // 环境光
    vec3 lighting = Albedo * 0.1;

    vec3 viewDir = normalize(viewPos - FragPos);
    for(int i = 0; i < NUM_OF_LIGHTS; ++i)
    {
        // 漫反射
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuseLighting = max(dot(Normal, lightDir), 0.0) * Albedo * lights[i].Color;
        
        // 镜面反射
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfwayDir, Normal), 0.0), 32);
        vec3 specularLighting = lights[i].Color * spec * specular;

        // 光照衰减
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);

        lighting += (diffuseLighting + specularLighting) * attenuation;
    }

    FragColor = vec4(lighting, 1.0);
}