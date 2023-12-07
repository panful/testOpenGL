#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 myColor;

uniform mat4 transform;

void main()
{
   myColor = aColor;

   gl_Position = transform * vec4(aPos, 1.0);

   // ID是VBO中的顶点索引，和EBO没有关系
   // 调用glDrawElements时，gl_VertexID有可能不从0开始，例如本例VAO2的索引{3,2,1}就是从1开始
   // 调用glDrawArrays时，是从0开始计数，gl_VertexID实际就是从调用该函数开始已处理的顶点个数
   gl_PointSize = gl_VertexID + 1;
}