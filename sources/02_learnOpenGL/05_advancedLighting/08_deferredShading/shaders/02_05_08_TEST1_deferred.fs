#version 330 core

layout (location = 0) out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D aTtexture;

void main()
{
    FragColor = vec4(texture(aTtexture, TexCoords).rgb, 1.0);
}