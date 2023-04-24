#version 330 core

out vec4 FragColor;
in float TexCoord;

// 1D纹理的采样器为 sampler1D
uniform sampler1D myTexture;

void main()
{
    // 1D纹理只有宽度，所以纹理坐标也只需要一个值
    FragColor = texture(myTexture, TexCoord);
}
