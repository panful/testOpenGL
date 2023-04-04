#version 330 core

layout(lines) in;
layout(line_strip, max_vertices = 6) out;

// 几何着色器与顶点着色器之间传递数据必须使用数组
// 因为几何着色器输入的是图元，图元可以有大于1个顶点
// 例如vs使用 out vec3 vColor; gs应该使用 in vec3 vColor[];

// 接口块的使用可以查看02_04_08_TEST6
in _VS_OUT_
{
    vec3 color;
}my_gs_in[];

out vec3 fColor;

void main()
{
    // 该fColor之后所有的顶点的颜色都将是fColor，直到下一次设置fColor的值
    fColor = (my_gs_in[0].color + my_gs_in[1].color) / 2;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    // EndPrimitive()有点类似于图元重启（02_01_06_TEST4）
    // 前面的顶点不会和后面的顶点连在一起
    // EndPrimitive()之前的顶点是一个图元，后面的是另一个（或多个）图元
    // 可以将EndPrimitive()注释掉来比较区别
    EndPrimitive();

    fColor = vec3(1.0);
    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0.0, 0.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.1,-0.1, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();

    fColor = my_gs_in[0].color;
    gl_Position = gl_in[1].gl_Position + vec4(-0.1,0.1, 0.0, 0.0);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + vec4(-0.1,-0.1, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}