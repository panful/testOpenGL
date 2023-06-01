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
uniform bool gamma;
uniform bool attenuation;

void main()
{
    vec3 color = texture(theTexture, TextureCoord).rgb;

    // ambient
    vec3 ambient = light.ambient;

    //  diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir,normal), 0.);
    vec3 diffuse = light.diffuse * diff * color;

    // specular
    vec3 viewDir = normalize(viewPos-FragPos);
    float spec = 0.f;
    float shininess = 1.f;

    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir+viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.), shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    }
    vec3 specular = light.specular * spec * color;

    vec3 lighting = ambient + diffuse + specular;

    // gamma校正
    if(gamma)
    {
        float gammaValue = 2.2;
        lighting = pow(lighting, vec3(1.0 / gammaValue));
    }

    // 光照衰减
    if(attenuation)
    {
        float dist = length(light.position - FragPos);

        if(gamma)
        {
            lighting *= (1.0 / dist * dist);
        }
        else
        {
            lighting *= (1.0 / (dist));
        }
    }

    FragColor = vec4(lighting, 1.0);
}