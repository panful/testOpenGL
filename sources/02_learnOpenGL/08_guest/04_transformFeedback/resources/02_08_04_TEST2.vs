#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 nextPos;

void main()
{
    nextPos = vec3(-aPos.x, aPos.yz);
    gl_Position = vec4(aPos, 1.0);
}