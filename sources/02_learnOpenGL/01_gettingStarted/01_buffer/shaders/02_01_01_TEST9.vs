#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
out vec3 Color;

void main()
{
    Color = aColor;
    gl_Position = vec4(aPos, 0.f, 1.f);
}
