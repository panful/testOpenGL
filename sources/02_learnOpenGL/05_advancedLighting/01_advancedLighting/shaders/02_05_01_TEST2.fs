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

uniform sampler2D theTexture;
uniform Light light;
uniform vec3 viewPos;
uniform bool blinn;

void main()
{
    vec3 color = texture(theTexture, TextureCoord).rgb;

    // // ambient
     vec3 ambient = light.ambient;

    // // diffuse
     vec3 lightDir = normalize(light.position - FragPos);
     vec3 normal = normalize(Normal);
     float diff = max(dot(lightDir,normal), 0.);
     vec3 diffuse = light.diffuse * diff * color;

    // specular
    vec3 viewDir = normalize(viewPos-FragPos);
    float  spec = 0.f;
    // 反光度，越高反射光的能力越强，散射的越少，高光点就会越小
    float shininess = 8.f;

    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir+viewDir);
        // 将blinn-phong光照模型的镜面反光度设置为phong模型的4倍
        spec = pow(max(dot(normal, halfwayDir), 0.), shininess * 4.f);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    }
    vec3 specular = light.specular * spec * color;

    FragColor = vec4(ambient + diffuse + specular, 1.);
}