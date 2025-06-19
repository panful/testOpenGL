#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 gStartPos;
out vec2 gScreenPos;

uniform float uLineWidth;
uniform vec2 uResolution;

void main()
{
    vec3 clipPos[2];
    for(int i = 0; i < 2; ++i)
    {
        // 透视除法 => NDC 坐标
        clipPos[i] = gl_in[i].gl_Position.xyz / gl_in[i].gl_Position.w;
    }

    vec2 dir = normalize(clipPos[1].xy - clipPos[0].xy);
    vec2 normal = vec2(-dir.y, dir.x); // 计算法线方向

    // 计算NDC中每个像素的偏移量
    vec2 pixelOffset = normal / uResolution * 2.0; // 2.0是因为NDC范围[-1,1]
    // NDC空间下，沿normal方向生长 uLineWidth/2 像素的偏移量
    vec2 ndcOffset = pixelOffset * uLineWidth * 0.5;

    // 直线起始点，只需要发送一次
    gStartPos = (clipPos[0].xy + vec2(1.0)) * 0.5 * uResolution;
    
    gScreenPos = gStartPos;
    gl_Position = vec4(clipPos[0].xy + ndcOffset, clipPos[0].z, 1.0);
    EmitVertex();

    gl_Position = vec4(clipPos[0].xy - ndcOffset, clipPos[0].z, 1.0);
    EmitVertex();

    gScreenPos = (clipPos[1].xy + vec2(1.0)) * 0.5 * uResolution;
    gl_Position = vec4(clipPos[1].xy + ndcOffset, clipPos[1].z, 1.0);
    EmitVertex();

    gl_Position = vec4(clipPos[1].xy - ndcOffset, clipPos[1].z, 1.0);
    EmitVertex();

    EndPrimitive();
}
