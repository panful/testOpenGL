#version 330 core

layout (location = 0) out vec4 FragColor0;
layout (location = 1) out vec4 FragColor1;

void main()
{
    FragColor0 = vec4(0.f, 0.f, 1.f, 1.f);
    FragColor1 = vec4(0.f, 1.f, 0.f, 1.f);
}
