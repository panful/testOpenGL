#version 450 core
layout (vertices = 3) out;
  
in vec4 out_Pos[];
in vec3 out_Color[];
  
out vec4 tcs_pos[];
out vec3 tcs_color[];

void main()
{
    gl_TessLevelInner[0] = 3;

    gl_TessLevelOuter[0] = 2;
    gl_TessLevelOuter[1] = 3;
    gl_TessLevelOuter[2] = 4;
  
    tcs_pos[gl_InvocationID]   = out_Pos[gl_InvocationID];
    tcs_color[gl_InvocationID] = out_Color[gl_InvocationID];
}