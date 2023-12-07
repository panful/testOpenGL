#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 myColor;

uniform mat4 transform;

void main()
{
   myColor = aColor;

   gl_Position = transform * vec4(aPos, 1.0);

   // 需要在代码中开启GL_PROGRAM_POINT_SIZE才可以在顶点着色器设置该值
   // gl_PointSize = 5.0;

   // 将顶点的大小设置为裁剪空间的z值，当顶点距离观察者越远时，顶点越大
   // @note 此处没有考虑z为负值
   gl_PointSize = gl_Position.z * 10.0;
}