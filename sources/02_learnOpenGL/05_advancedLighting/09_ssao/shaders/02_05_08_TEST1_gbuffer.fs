#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

void main()
{
    gPosition.xyz = fs_in.FragPos;
    gNormal = fs_in.Normal;
    gAlbedo.rgb = vec3(1.0);
}