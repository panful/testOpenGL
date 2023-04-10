#version 330 core

layout(triangles) in;
layout(line_strip, max_vertices = 2) out;

out vec3 fColor;

void main()
{
    // gl_in.length()返回的是输入数据的顶点个数
    // triangles顶点个数为3

    if(gl_in.length() == 3)
        fColor = vec3(1.0, 0.0, 0.0);
    else if(gl_in.length() > 3)
        fColor = vec3(0.0, 1.0, 0.0);
    else
        fColor = vec3(0.0, 0.0, 1.0);

    gl_Position = (gl_in[0].gl_Position + gl_in[2].gl_Position) / 2;
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + gl_in[1].gl_Position) / 2;
    EmitVertex();
    
    EndPrimitive();
}