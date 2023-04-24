#version 330 core

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D uTex;

void main()
{
    vec4 texColor = texture(uTex, texCoord);
    if(texColor.a < 0.1)
    {
        // 当alpha小于0.1时略过该片段
        discard;
    }

    FragColor = texColor;

    //FragColor = texture(uTex, texCoord);
}
