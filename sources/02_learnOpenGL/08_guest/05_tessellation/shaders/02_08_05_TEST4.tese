#version 450 core

layout(quads, equal_spacing, ccw) in;

out vec3 out_Color;
  
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2, vec3 v3)
{
    // gl_TessCoord 当前处理的顶点在输入的面片上的纹理坐标，范围为[0, 1]
    float s = gl_TessCoord.x;
    float t = gl_TessCoord.y;

    // 双线性插值(bilinear interpolation)
    return (1 - s) * (1 - t) * v0 + s * (1 - t) * v1 + s * t * v2 + (1 - s) * t * v3;
}

// 使用指定的四个颜色
vec3 colors[] = vec3[4](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 1.0)
);

void main(){
    vec3 iterP = interpolate3D(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz, gl_in[3].gl_Position.xyz);
    vec3 iterCd = interpolate3D(colors[0], colors[1], colors[2], colors[3]);

    out_Color = iterCd;
    gl_Position = vec4(iterP , 1.0);
}