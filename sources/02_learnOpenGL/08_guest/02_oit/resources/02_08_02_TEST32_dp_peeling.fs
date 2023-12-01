#version 330 core

uniform sampler2D texDepth;
uniform vec4 uColor;

out vec4 FragColor;

void main()
{
    float up_depth = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0).r;

    if(gl_FragCoord.z <= up_depth)
    {
        // 深度值不大于上一次pass，将该片段的深度值设置为1.f，这样该片段就永远不会通过深度测试
        // 或者使用discard直接将该片段丢弃
        // gl_FragDepth = 1.f;
        discard;
    }
    else
    {
        gl_FragDepth = gl_FragCoord.z;
        FragColor = uColor;
    }
}

