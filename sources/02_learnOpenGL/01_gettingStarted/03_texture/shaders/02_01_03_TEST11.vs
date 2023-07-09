#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_PointSize = 40;
    gl_Position = vec4(aPos, 1.0);
}