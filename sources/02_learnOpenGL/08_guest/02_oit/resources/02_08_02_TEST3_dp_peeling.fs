#version 330 core

//uniform sampler2D texColor;
uniform sampler2D texDepth;
uniform vec4 uColor;

layout (location = 0) out vec4 FragColor;

void main()
{
    //vec4  up_color = texelFetch(texColor, ivec2(gl_FragCoord.xy), 0);
    float up_depth = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0).r;

    FragColor = uColor;
    if(gl_FragCoord.z <= up_depth)
    {
        gl_FragDepth = 1.f;
    }
    else
    {
        // vec3 color_rgb = up_color.rgb * up_color.a + (1 - up_color.a) * uColor.rgb * uColor.a;
        // float color_a  = up_color.a - up_color.a * uColor.a + uColor.a;
        // FragColor      = vec4(color_rgb, color_a);
        gl_FragDepth   = gl_FragCoord.z;
    }
}

