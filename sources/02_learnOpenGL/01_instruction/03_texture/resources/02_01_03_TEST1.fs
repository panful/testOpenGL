#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
    // 只使用纹理
    FragColor = texture(texture1, TexCoord);

    // 纹理和颜色叠加
    //FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
}
