#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light {
    vec3 direction;
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

uniform int pointLight;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TextureCoord).rgb;
  	
    // diffuse 
    vec3 lightDir = vec3(1.f);

    if(pointLight == 0)
    {
        // 平行光，light.direction是光源指向的方向
        lightDir = normalize(-light.direction);
    }
    else if(pointLight == 1)
    {
        // 点光源，light.direction是光源的位置
        lightDir = normalize(light.direction - FragPos);
    }

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