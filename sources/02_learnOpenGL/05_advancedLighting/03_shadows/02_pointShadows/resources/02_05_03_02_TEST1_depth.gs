#version 330 core

layout (triangles) in;

// 输入的是一个三角形（立方体由6个面组成，面由2个三角形组成）
// 输出6个方向上的三角形，共3*6 = 18个顶点
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face;

        for(int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }

        EndPrimitive();
    }
}