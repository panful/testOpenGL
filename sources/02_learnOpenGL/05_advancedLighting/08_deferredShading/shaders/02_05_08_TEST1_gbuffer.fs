#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

// 漫反射（图元的颜色，此处纹理就是颜色）
uniform sampler2D diffuse;
// 镜面反射强度（镜面反射和图元的颜色没有关系，只和光照颜色有关）
uniform float specular;

void main()
{
    gPosition = fs_in.FragPos;
    gNormal = fs_in.Normal;
    gAlbedoSpec.rgb = texture(diffuse, fs_in.TexCoords).rgb;
    gAlbedoSpec.a = specular;
}