#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

// 接口块（Interface Block）类似于结构体（struct）
// _VS_OUT_需要和接收该接口块的着色器中定义的接口块名称相同
// 例如vs中的接口块名为：_VS_OUT_ fs中的接口块名称也为：_VS_OUT_
// 实例名：my_vs_out可以随意，只在当前的着色器中使用
// 成员名：myColor需要和接收的名称一致，类似接口块名
out _VS_OUT_
{
   vec3 myColor;
}my_vs_out;

uniform mat4 transform;

void main()
{
   gl_Position = transform * vec4(aPos, 1.0);

   my_vs_out.myColor = aColor;
}