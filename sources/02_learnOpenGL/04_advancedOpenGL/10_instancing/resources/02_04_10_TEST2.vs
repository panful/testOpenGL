#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aOffset;

out vec4 color;

void main()
{
    gl_Position = vec4(aPos.x + aOffset , aPos.y, 0.f, 1.f);
    color = vec4(aColor / (gl_InstanceID + 1), 1.f);
}