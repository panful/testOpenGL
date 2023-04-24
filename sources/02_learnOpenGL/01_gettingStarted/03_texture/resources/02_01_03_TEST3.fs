#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture_1;
uniform sampler2D texture_2;

void main()
{
    // 只使用纹理
    FragColor = texture(texture_1, TexCoord) * texture(texture_2,TexCoord);

    //FragColor = mix(texture(texture_1, TexCoord), texture(texture_2, TexCoord), .1f);

    // 纹理和颜色叠加
    //FragColor = texture(texture_1, TexCoord) * vec4(ourColor, 1.0);
}
