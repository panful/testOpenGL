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

uniform int shadowSoft = 0;

float CalculationShadow(vec3 fragPos)
{
    // 偏移值、采样范围等都需要根据具体场景微调
    float shadow = 0.0;
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    if(shadowSoft == 0)
    {
        float bias = 0.005;
        float closestDepth = texture(depthMap, fragToLight).r;
        closestDepth *= far_plane;
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
    else if(shadowSoft == 1)
    {
        float bias = 0.005;
        float offset = 0.1;
        float samples = 4.0;

        for(float x = -offset; x < offset; x += offset / (samples / 2.0))
        {
            for(float y = -offset; y < offset; y += offset / (samples / 2.0))
            {
                for(float z = -offset; z < offset; z += offset / (samples / 2.0))
                {
                    float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r;
                    closestDepth *= far_plane;

                    if(currentDepth - bias > closestDepth)
                    {
                        shadow += 1.0;
                    }
                }
            }
        }

        shadow /= (samples * samples * samples);
    }
    else
    {
        const int samples = 20;

        vec3 sampleOffsetDirections[samples] = vec3[]
        (
            vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
            vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
            vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
            vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
            vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
        );

        float bias = 0.2;
        float viewDistance = length(viewPos - fragPos);
        float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0; // 距离观察者越远阴影越柔和

        for(int i = 0; i < samples; ++i)
        {
            float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
            closestDepth *= far_plane;

            if(currentDepth - bias > closestDepth)
            {
                shadow += 1.0;
            }
        }

        shadow /= samples;
    }

    return shadow;
}

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
