#version 450 core

// 设置输出面片顶点的数量，即gl_out的数量
// 还可以控制细分控制着色器的执行次数，每个输出面片顶点执行一次
layout (vertices = 4) out;

in vec3 out_Color[];
out vec3 tcs_color[];

// gl_in是一个如下的结构体数组
// in gl_PerVertex {
//     vec4 gl_Position;
//     float gl_PointSize;
//     float gl_ClipDistance[];
// }gl_in[gl_PatchVerticesIn];

// gl_out与gl_in结构体相同，但是大小不一样

// gl_InvocationID 当前处理的是哪个顶点，gl_out数组的当前索引
// gl_PrimitiveID  当前输入面片的图元索引
// gl_PatchVerticesIn  输入面片的顶点数量，即gl_in的大小
// gl_PatchVerticesOut 输出面片的顶点数量，即gl_out的大小

// gl_TessLevelOuter 决定外围的细分等级，数组大小为4
// gl_TessLevelInner 决定内部的细分等级，数组大小为2
// 其数值代表将该段分为几部分，即如果值为2，那么就是在中间插入1个点分为两段
// 可以使用 glPatchParameterfv 函数在外部设置这两个值
// 根据划分的图元不同，这两个数组的某些数值可能并不会用到
// 注意这两个数组的类型是浮点数，小数点后的部分应该会影响外侧坐标的生成

void main()
{
    gl_TessLevelInner[0] = 3.0;
    gl_TessLevelInner[1] = 3.0;

    gl_TessLevelOuter[0] = 4.0;
    gl_TessLevelOuter[1] = 4.0;
    gl_TessLevelOuter[2] = 4.0;
    gl_TessLevelOuter[3] = 4.0;
  
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcs_color[gl_InvocationID] = out_Color[gl_InvocationID];
}