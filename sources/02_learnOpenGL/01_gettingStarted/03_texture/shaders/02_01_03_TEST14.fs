#version 330 core
out vec4 FragColor;

// 注意此处是 vec3
in vec3 TexCoord;

uniform sampler2DArray texArray;

void main()
{
    FragColor = texture(texArray, TexCoord);
}
