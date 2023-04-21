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
    vec3 position;      // 位置
    vec3 direction;     // 方向
    float cutOff;       // 聚光灯圆锥角度余弦值
    // 聚光灯外圆锥角度余弦值
    // 从内圆锥到外圆锥之间的片段会平滑过渡光照强度
    // 从而实现平滑聚光灯边缘的效果
    float outerCutOff;

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
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TextureCoord).rgb;
    
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TextureCoord).rgb;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TextureCoord).rgb;

    // 平滑聚光灯边缘
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    // clamd(x,y,z) = min(max(x,y),z) intensity是一个[0,1]范围内的值
    // 对于大圆锥以外的片段intensity将等于0，小圆锥以内的片段intensity将等于1
    // 大圆锥到小圆锥之间的区域，intensity将从0过渡到1
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // 对漫反射和镜面反射做平滑，环境光不做平滑，让环境总是有一点光
    diffuse *= intensity;
    specular *= intensity;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

    // 对intensity可视化
    // if(intensity < 0.0001)
    //     FragColor =  vec4(1.f);
    // else if(intensity > 0.9999)
    //     FragColor = vec4(0.f);
    // else
    //     FragColor = vec4(1.f, 0.f, 0.f, 1.f);
} 