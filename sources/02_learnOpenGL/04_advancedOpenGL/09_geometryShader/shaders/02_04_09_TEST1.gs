#version 330 core

// layout()的参数可以为以下值，后面的类型表示调用glDrawArrays或glDrawElements时的图元类型
// 后面的括号中的数字表示该类型的图元需要的最少顶点个数
// points：             GL_POINTS（1）。
// lines：              GL_LINES、GL_LINE_STRIP（2）
// lines_adjacency：    GL_LINES_ADJACENCY、GL_LINE_STRIP_ADJACENCY（4）
// triangles：          GL_TRIANGLES、GL_TRIANGLE_STRIP、GL_TRIANGLE_FAN（3）
// triangles_adjacency：GL_TRIANGLES_ADJACENCY、GL_TRIANGLE_STRIP_ADJACENCY（6）
layout(points) in;

// 第一个参数表示新生成的图元类型，可以是以下任意值
// points
// line_strip
// triangle_strip
// 第二个参数表示新生成的图元的顶点个数，如果超过了这个值，OpenGL将不会绘制多出的顶点
// 公用的顶点算一个顶点，例如两个三角形4个顶点而不是6个
layout(triangle_strip, max_vertices = 3) out;

// gl_in可以看作是一个如下的接口块（glsl内部的变量），
// 因为输入是一个GL_POINTS，所以gl_in数组的大小为1，
// 如果是GL_LINES，gl_in的大小就为2
// in gl_Vertex
// {
//     vec4  gl_Position;
//     float gl_PointSize;
//     float gl_ClipDistance[];
// } gl_in[];

void main()
{
    // gl_Position是新生成的顶点，连接顶点时只包含新生成的，从顶点着色器输出的顶点并不会绘制
    // gl_in是几何着色器的输入接口快
    // gl_in[0].gl_Position是顶点着色器的输出顶点（GL_POINTS只有一个顶点）
    // EmitVertex()每调用一次，将gl_Position的向量添加到几何着色器的输出图元中
    gl_Position = gl_in[0].gl_Position + vec4(-0.3, -0.3, 0.0, 0.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.3, -0.3, 0.0, 0.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.3, 0.0, 0.0);
    EmitVertex();

    // 调用EndPrimitive()，之前使用EmitVertex()发送的顶点会被合并成一个输出图元
    EndPrimitive();
}