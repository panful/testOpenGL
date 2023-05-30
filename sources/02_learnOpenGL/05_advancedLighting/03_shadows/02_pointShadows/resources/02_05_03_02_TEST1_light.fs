#version 330 core

out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;

float CalculationShadow(vec3 fragPos)
{
    float bias = 0.005;

    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(depthMap, fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

// 一定不能忘记标准化方向向量

void main()
{
    vec3 lightColor = vec3(1.0);
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 norm = normalize(fs_in.Normal);

    vec3 ambient = 0.1 * lightColor;

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    float shadow = CalculationShadow(fs_in.FragPos);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}
