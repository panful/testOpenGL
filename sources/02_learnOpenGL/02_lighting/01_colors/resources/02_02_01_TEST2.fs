#version 330 core

out vec4 FragColor;
//flat in vec3 ourColor;
smooth in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0f);
}