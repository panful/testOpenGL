#version 330 core

in vec3 vsOutColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vsOutColor, 1.0);
}
