#version 330 core

uniform sampler2D tex_water;
uniform sampler2D tex_flowmap;

in vec2 TexCoord;
out vec4 FragColor;

uniform float time;
uniform float flowstrength; // 流动强度，越大流动越快，最好不要超过1

void main()
{
    // 取flowmap的rg分量，并将rg分量从[0,1]转换到[-1,1]
    vec2 motionvector = texture(tex_flowmap, TexCoord).xy * 2. - 1.;

    // 两个相位相差半个周期的UV运动，实现永不停息的流动
    vec2 uv1 = TexCoord + fract(time) * motionvector * flowstrength;
    vec2 uv2 = TexCoord + fract(time + 0.5) * motionvector * flowstrength;

    vec3 color1 = texture(tex_water, uv1).rgb;
    vec3 color2 = texture(tex_water, uv2).rgb;

    // 第一个图像的uv运动在当前周期快要结束的时候，逐渐淡出
    // 第二个图像的uv运动在周期起始位置开始，逐渐淡入
    float alpha = abs((fract(time) - 0.5) * 2.); // 模拟三角波，也可以使用sin函数模拟
    vec3 color = mix(color1, color2, alpha);

    FragColor = vec4(color.rgb, 1.);
}
