#version 400

// invocations 表示调用几次，glsl版本400以后才支持
layout (triangles, invocations = 2) in;
layout (triangle_strip, max_vertices = 3) out;

void main()
{
    for(int i = 0; i < 2; i++)
    {
        // gl_InvocationID 表示第几次调用（第几个图层）
        gl_Layer = gl_InvocationID;

        gl_Position = gl_in[0].gl_Position + vec4(vec3(0.2, 0.2, 0.2) * gl_InvocationID, 0.0);
        EmitVertex();
        gl_Position = gl_in[1].gl_Position + vec4(vec3(0.2, 0.2, 0.2) * gl_InvocationID, 0.0);
        EmitVertex();
        gl_Position = gl_in[2].gl_Position + vec4(vec3(0.2, 0.2, 0.2) * gl_InvocationID, 0.0);
        EmitVertex();

        EndPrimitive();
    }
}