#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light{
    vec3 Position;
    vec3 Color;
};

uniform sampler2D theTexture;
uniform Light lights;

void main()
{
    vec3 color = texture(theTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);

    // ambient
    vec3 ambient = 0.0 * color;

    vec3 lighting = vec3(0.0);

    // diffuse
    vec3 lightDir = normalize(lights.Position - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lights.Color * diff * color;

    vec3 result = diffuse;
    float dist = length(fs_in.FragPos - lights.Position);
    result *= 1.0 / (dist * dist);
    lighting += result;

    FragColor = vec4(ambient + lighting, 1.0);
}