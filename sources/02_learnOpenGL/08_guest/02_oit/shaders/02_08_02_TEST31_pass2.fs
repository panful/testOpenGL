#version 450

uniform sampler2D texDepth; // 距离相机最近的片段的深度值
uniform vec4 uColor;

out vec4 FragColor;

void main()
{
    ivec2 pixelCoord = ivec2(gl_FragCoord.xy);
    float depth = texelFetch(texDepth, pixelCoord, 0).r;

    // 需要给depth加一个足够大的容差，保证不出现Z冲突
    if(gl_FragCoord.z > depth + 0.000001)
    {
        gl_FragDepth = gl_FragCoord.z;
    }
    else
    {
        // 当前片段的深度值不大于texDepth深度值的时候，
        // 将当前片段的深度值设置为1.0，在开启深度测试且深度测试比较方式为GL_LESS时
        // 当前片段就会被丢弃（不能通过深度测试），即距离相机最近的片段被丢弃
        // 此时输出到颜色缓冲的片段就是距离相机第二近的片段
        gl_FragDepth = 1.f;
    }

    FragColor = uColor;
}

