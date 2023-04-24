#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out _VS_OUT_
{
   vec3 color;
}my_vs_out;

void main()
{
   my_vs_out.color = aColor;
   gl_Position = vec4(aPos, 1.0);
}