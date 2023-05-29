#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

// 是否开启阴影
uniform bool IsShadow;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;

    // 当坐标超出光的正交视锥的远平面时，将shadow的值始终设置为0.0
    // 这样有一个缺陷：当超出光的视锥远平面时，不能处于阴影中
    if(1.0 < projCoords.z)
    {
        return 0.0;
    }

    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
   
    // 检查当前片段是否在阴影中
    // 减0.00001是为了解决阴影失真的问题，因为距离是一个浮点数，不容易比较
    // 需要引入一个比较误差，这是数值精度的问题，不能从本质解决问题
    // 这个值需要根据具体场景进行微调
    float shadow = (currentDepth - 0.00001) > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);

    // Ambient
    vec3 ambient = 0.15 * color;

    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    vec3 lighting = vec3(1.f);

    if(IsShadow)
    {   
        // 计算阴影
        // 相比于普通的BLINN-PHONG，多了一个判断当前片段是否在阴影中的步骤
        // 如果当前片段的z值比shadowMap中的值大，则该片段在阴影中
        float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
        lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    }
    else
    {
        lighting = (ambient + diffuse + specular) * color;
    }

    FragColor = vec4(lighting, 1.0f);
}