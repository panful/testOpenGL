#version 330 core

out vec4 FragColor;

in vec3 myColor;

void main()
{
     //FragColor = vec4(myColor, 1.f);

     // gl_FrontFacing如果为true，当前片段是属于正向面的一部分
     // 开启背面剔除就看不到立方体内部的面，使用gl_FrontFacing就没有意义
     if(gl_FrontFacing)
          FragColor = vec4(1.f, 0.f, 0.f, 1.f);
     else
          FragColor = vec4(0.f, 1.f, 0.f, 1.f);
}