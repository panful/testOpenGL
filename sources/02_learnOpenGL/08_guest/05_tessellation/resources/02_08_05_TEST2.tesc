#version 450 core
layout (vertices = 3) out;
  
in vec4 out_Pos[];
in vec3 out_Color[];
  
out vec4 tcs_pos[];
out vec3 tcs_color[];

// gl_TessLevelOuter 决定外围的细分等级，分为上下左右四个方向，即它的大小为4
// gl_TessLevelInner 决定内部的细分等级，分为横竖两个方向，即它的大小为2
// 其数值代表将该段分为几部分，即如果值为2，那么就是在中间插入1个点分为两段
// 可以使用 glPatchParameterfv 函数在外部设置这两个值，这样就不需要曲面细分控制着色器了（如果没有其他需求）

// gl_InvocationID 当前细分曲面着色器的输出顶点的调用索引
// 几何着色器中也有这个变量，注意区分

void main()
{
    gl_TessLevelInner[0] = 3;

    gl_TessLevelOuter[0] = 2;
    gl_TessLevelOuter[1] = 3;
    gl_TessLevelOuter[2] = 4;
  
    tcs_pos[gl_InvocationID]   = out_Pos[gl_InvocationID];
    tcs_color[gl_InvocationID] = out_Color[gl_InvocationID];
}