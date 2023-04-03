#version 420 core

layout (location = 0) in vec3 aPos;

layout (std140, binding = 6) uniform MatVP
{
   mat4 view;
   mat4 projection;
};

// 用来测试glGetUniformBlockIndex()函数的返回值
layout (std140, binding = 7) uniform Test
{
   float test_float;
   int test_int;
};

uniform mat4 model;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
}