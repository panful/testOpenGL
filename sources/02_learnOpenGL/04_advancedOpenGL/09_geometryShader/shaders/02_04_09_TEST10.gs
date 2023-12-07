#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 posInView[];

// 三角形边缘方程
out vec4 edgeEqn[3];

// 窗口的Viewport
uniform vec4 vpDims;
// 边框的宽度
uniform float lineWidth;
// 用来确认当前边是否是多边形的边框
uniform samplerBuffer edgeTexture;

void main()
{
    vec2 pos[4];

    for(int i = 0; i < 3; ++i)
    {
        // 透视除法
        pos[i] = gl_in[i].gl_Position.xy / gl_in[i].gl_Position.w;
        // 坐标从 [-1, 1] 转换到 [0, 1]
        pos[i] = pos[i] * vec2(0.5) + vec2(0.5);
        // [0, 1] 转换为屏幕坐标
        pos[i] = pos[i] * vpDims.zw + vpDims.xy;
    }
    // 闭合的三角形
    pos[3] = pos[0];

    // 三角形顶点顺序为逆时针：ccw = 1，顺时针：ccw = -1
    float ccw = sign(cross(vec3(pos[1] - pos[0], 0.0), vec3(pos[2] - pos[0], 0.0)).z);

    for(int i = 0; i < 3; ++i)
    {
        // 当前边的方向
        vec2 tmp = normalize(pos[i+1] - pos[i]);
        // 当前边的法向量
        tmp = ccw * vec2(-tmp.y, tmp.x);
        // 平面方程的常数项
        float d = dot(pos[i], tmp);
        // 平面方程
        edgeEqn[i] = vec4(tmp.x, tmp.y, 0.0, -d);
    }

    // 三角形顶点偏移量
    vec2 offsets[3];

    // 三角形第0个顶点是第0条边和第2条边的交点
    offsets[0] = edgeEqn[2].xy + edgeEqn[0].xy;
    offsets[0] = -0.5 * normalize(offsets[0]) * lineWidth;
    offsets[0] /= vpDims.zw;

    // 三角形第1个顶点是第0条边和第1条边的交点
    offsets[1] = edgeEqn[0].xy + edgeEqn[1].xy;
    offsets[1] = -0.5 * normalize(offsets[1]) * lineWidth;
    offsets[1] /= vpDims.zw;

    offsets[2] = edgeEqn[1].xy + edgeEqn[2].xy;
    offsets[2] = -0.5 * normalize(offsets[2]) * lineWidth;
    offsets[2] /= vpDims.zw;

    float edgeValues = 255.0 * texelFetch(edgeTexture, gl_PrimitiveIDIn).r;

    // 三位二进制'000'分别表示三角形的第i条边

    // edgeValues的最大值为 7(7 = 0b111)小于4，则表示第三条边不是多边形的边框
    if(edgeValues < 4.0)
    {
        edgeEqn[2].z = lineWidth;
    }
    // 第二条边不是多边形的边框
    if(mod(edgeValues, 4.0) < 2.0)
    {
        edgeEqn[1].z = lineWidth;
    }
    // 第一条边不是多边形的边框
    if(mod(edgeValues, 2.0) < 1.0)
    {
        edgeEqn[0].z = lineWidth;
    }

    for(int i = 0; i < 3; ++i)
    {
        gl_PrimitiveID = gl_PrimitiveIDIn;

        gl_Position    = gl_in[i].gl_Position;
        gl_Position.xy = gl_Position.xy + offsets[i] * gl_Position.w;
    
        EmitVertex();
    }

    EndPrimitive();
}