#version 330 core

out vec4 FragColor;
in vec2 vsOutTexCoord;
uniform vec4 uColor;

void main()
{    
    FragColor = uColor;
}