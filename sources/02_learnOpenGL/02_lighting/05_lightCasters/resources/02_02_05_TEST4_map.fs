#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 

struct Light {
    // 聚光灯
    vec3 position;  // 位置
    vec3 direction; // 方向
    float cutOff;   // 聚光灯圆锥角度余弦值

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TextureCoord;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 lightDir = normalize(light.position - FragPos);
    // 【片段到光源的向量】与【聚光灯方向】的夹角的点乘（余弦值）
    float theta = dot(lightDir, normalize(-light.direction));

    // 如果片段在聚光灯圆锥内部，则正常显示
    if(theta > light.cutOff)
    {
        // ambient
        vec3 ambient = light.ambient * texture(material.diffuse, TextureCoord).rgb;
        
        // diffuse 
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TextureCoord).rgb;
        
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * texture(material.specular, TextureCoord).rgb;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
    else
    {
        // 如果片段在聚光灯圆锥的外部，则只显示材质的漫反射颜色（没有灯光会比较暗）
        FragColor = vec4(light.ambient * texture(material.diffuse, TextureCoord).rgb, 1.f);
    }
} 