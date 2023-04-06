#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

// 只需要设置 location = 2其他三个不用声明，location会默认递增
layout(location = 2) in mat4 model;

out vec3 myColor;

// uniform mat4 model;

void main()
{
    gl_Position = model * vec4(aPos, 0.f, 1.f);
    myColor = aColor;
}