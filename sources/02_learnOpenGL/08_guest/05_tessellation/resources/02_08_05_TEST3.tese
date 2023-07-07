#version 450 core

// 第一个参数 图元类型
// quads    四边形 gl_TessCoord包含xy的类似纹理坐标
// triangles三角形 gl_TessCoord包含xyz的重心坐标
// isolines 等值线 gl_TessCoord包含xy的类似纹理坐标，x范围[0,1] y的范围从0到接近于1
// 第二个参数 控制细分坐标的间隔，控制的是外侧细分层级的小数值，内侧坐标不会受该参数影响
// equal_spacing 
// fractional_even_spacing 
// fractional_odd_spacing
// 第三个参数 顶点的排列顺序
// ccw逆时针 cw顺时针
layout(quads, equal_spacing, ccw) in;

in vec3 tcs_color[];
out vec3 out_Color;
  
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2, vec3 v3)
{
    // gl_TessCoord 当前处理的顶点在输入的面片上的纹理坐标，范围为[0, 1]
    float s = gl_TessCoord.x;
    float t = gl_TessCoord.y;

    // 双线性插值(bilinear interpolation)
    return (1 - s) * (1 - t) * v0 + s * (1 - t) * v1 + s * t * v2 + (1 - s) * t * v3;
}

// 细分计算着色器也有一个gl_in结构体数组，和细分控制着色器结构体是一样的
 
void main(){
    vec3 iterP = interpolate3D(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz, gl_in[3].gl_Position.xyz);
    vec3 iterCd = interpolate3D(tcs_color[0], tcs_color[1], tcs_color[2], tcs_color[3]);
      
    out_Color = iterCd;
    gl_Position = vec4(iterP , 1.0);
}