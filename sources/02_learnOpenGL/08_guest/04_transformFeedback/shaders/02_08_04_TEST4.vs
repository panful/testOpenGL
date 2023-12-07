#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 nextPos;

uniform float uTime;

void main()
{
    float radius = 0.5;
    nextPos = vec3(sin(uTime) * radius, cos(uTime) * radius, 0.0);
    gl_Position = vec4(aPos, 1.0);
}