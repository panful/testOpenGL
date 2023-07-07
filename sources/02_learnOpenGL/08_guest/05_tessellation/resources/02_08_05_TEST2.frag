#version 450 core
  
out vec4 FragColor;
in vec3  out_Color;

// 如果使用了细分着色器
// 顶点着色器的输出不能在片段着色器中直接使用，需要通过细分着色器输出到片段着色器

void main()
{
     FragColor = vec4(out_Color, 1.0);
}