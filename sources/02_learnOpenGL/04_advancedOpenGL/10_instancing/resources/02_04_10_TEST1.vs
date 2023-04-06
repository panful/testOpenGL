#version 330 core

layout (location = 0) in vec2 aPos;

out vec4 vsOutColor;

// 偏移数组，通过 gl_InstanceID 来索引每一个偏移量
// 在c++代码中通过glUniform1f循环10次设置每一个值
uniform float offsets[10];

void main()
{
   // gl_InstanceID表示目前绘制第几个实例，会从0开始，在每个实例被渲染时递增1
   // glDrawArraysInstanced() 或 glDrawElementsInstanced() 最后一个参数表示实例的个数
   float _offset_ = offsets[gl_InstanceID];

   vsOutColor = vec4(_offset_);
   gl_Position = vec4(aPos.x + _offset_, aPos.y, 0.0f, 1.0f);
}