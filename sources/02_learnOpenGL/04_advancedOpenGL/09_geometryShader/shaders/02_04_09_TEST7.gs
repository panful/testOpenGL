#version 450 core

// 输入的顶点位置不变，每个视口的顶点颜色不同
// invocations 表示实例化多少次，可以理解为调用几次几何着色器
// 实例化和顶点着色器的 gl_InstanceID 类似
layout (triangles, invocations = 4) in;
layout (triangle_strip, max_vertices = 3) out;
 
out vec3 fColor;
 
uniform mat4 model[4];

const vec3 colors[4] = vec3[4]
        (
            vec3(1.0, 0.7, 0.3),
            vec3(1.0, 0.2, 0.3),
            vec3(0.1, 0.6, 1.0),
            vec3(0.3, 0.7, 0.5)
        );

// 实例化输出的图元根据gl_InvocationID排序。因此如果用户渲染了两个图元，并设置invocations为3，
// 那么GS的有效调用顺序如下：(prim0,inst0),(prim0,inst1),(prim0,inst2),(prim1,inst0)…,
// GS的输出图元根据输入序列进行排序。因此如果(prim0,inst0)输出两个三角形，它们都会在渲染(prim0,inst1)的任何三角形之前被渲染。
void main()
{
    gl_ViewportIndex = gl_InvocationID;
    fColor = colors[gl_InvocationID];
    // gl_Layer和gl_ViewportIndex是GS的输出变量。因此，每次调用EmitVertex时，它们的值都会变为未定义。因此，每次循环输出时都必须设置这些变量。
    // 如果几何着色器中并没有写入gl_ViewportIndex,那么就等价于赋值为0。
    // 本示例 gl_ViewportIndex = gl_InvocationID; 在for循环内和循环外渲染结果一致
    for (int i = 0; i < gl_in.length(); i++)
    {
        gl_Position =  model[gl_InvocationID] * gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}