#version 330 core
out vec4 FragColor;

uniform samplerBuffer colorTexture;

void main()
{
    // 通过gl_PrimitiveID获取纹理的第N个颜色值
    // 关于gl_PrimitiveID的使用查看02_04_09_TEST9
    vec3 color = texelFetch(colorTexture, gl_PrimitiveID).rgb;
    FragColor = vec4(color, 1.0);
}
