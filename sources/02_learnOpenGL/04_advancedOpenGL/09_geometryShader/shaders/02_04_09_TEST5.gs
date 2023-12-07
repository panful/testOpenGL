#version 330 core

// 输出等于输入
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 fColor;

void main()
{
    // 当前输入图元的ID，几何着色器处理的图元索引，从0开始
    // 注意 gl_PrimitiveID 是几何着色器的输出变量，如果没有定义几何着色器，则 gl_PrimitiveID = gl_PrimitiveIDIn
    // 如果定义了几何着色器可显式地执行 gl_PrimitiveID = gl_PrimitiveIDIn
    // 也可以给 gl_PrimitiveID 赋值一个任意值
    fColor = vec3(1.0 / (gl_PrimitiveIDIn + 1), 0.0, 0.0);

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    
    EndPrimitive();
}