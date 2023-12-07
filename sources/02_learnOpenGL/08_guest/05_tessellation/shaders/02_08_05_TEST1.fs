#version 330 core

out vec4 FragColor;

in float Height;

void main()
{
    float h = (Height + 16.0) / 32.0;
    FragColor = vec4(h, h, h, 1.0);
}