#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float uPointSize;
uniform vec2 uResolution;

out vec2 fragUV;

void main()
{
    vec4 clip_center = vec4(gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w, 1.0);

    vec2 offset = vec2(uPointSize, uPointSize) / uResolution;

    // 左下
    fragUV = vec2(0.0, 0.0);
    gl_Position = clip_center + vec4(-offset.x, -offset.y, 0.0, 0.0);
    EmitVertex();

    // 左上
    fragUV = vec2(0.0, 1.0);
    gl_Position = clip_center + vec4(-offset.x, offset.y, 0.0, 0.0);
    EmitVertex();

    // 右下
    fragUV = vec2(1.0, 0.0);
    gl_Position = clip_center + vec4(offset.x, -offset.y, 0.0, 0.0);
    EmitVertex();

    // 右上
    fragUV = vec2(1.0, 1.0);
    gl_Position = clip_center + vec4(offset.x, offset.y, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
