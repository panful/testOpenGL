#version 330 core

uniform sampler2D texDepth;
uniform vec4 uColor;

out vec4 FragColor;

void main()
{
    float down_depth = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0).r;

    FragColor = uColor;
    if(gl_FragCoord.z >= down_depth)
    {
        discard;
        // gl_FragDepth = 0.f;
    }
    else
    {
        gl_FragDepth   = gl_FragCoord.z;
    }
}

