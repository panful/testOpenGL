#version 330 core

in vec2 texture_coords;
out vec4 FragColor;

uniform sampler2D pass0;
uniform sampler2D pass1;
uniform sampler2D pass2;
uniform sampler2D pass3;
uniform sampler2D pass4;
uniform sampler2D pass5;
uniform sampler2D pass6;
uniform sampler2D pass7;

void main()
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy);

    vec4 c0 = texelFetch(pass0, fragCoord, 0);
    vec4 c1 = texelFetch(pass1, fragCoord, 0);
    vec4 c2 = texelFetch(pass2, fragCoord, 0);
    vec4 c3 = texelFetch(pass3, fragCoord, 0);
    vec4 c4 = texelFetch(pass4, fragCoord, 0);
    vec4 c5 = texelFetch(pass5, fragCoord, 0);
    vec4 c6 = texelFetch(pass6, fragCoord, 0);
    vec4 c7 = texelFetch(pass7, fragCoord, 0);

    // 按照从最下面到最上面的顺序混合颜色
    vec3 color = vec3(0.f);
    color = c7.rgb * c7.a + (1.f - c7.a) * color.rgb;
    color = c6.rgb * c6.a + (1.f - c6.a) * color.rgb;
    color = c5.rgb * c5.a + (1.f - c5.a) * color.rgb;
    color = c4.rgb * c4.a + (1.f - c4.a) * color.rgb;
    color = c3.rgb * c3.a + (1.f - c3.a) * color.rgb;
    color = c2.rgb * c2.a + (1.f - c2.a) * color.rgb;
    color = c1.rgb * c1.a + (1.f - c1.a) * color.rgb;
    color = c0.rgb * c0.a + (1.f - c0.a) * color.rgb;

	FragColor = vec4(color, 1.f);
}