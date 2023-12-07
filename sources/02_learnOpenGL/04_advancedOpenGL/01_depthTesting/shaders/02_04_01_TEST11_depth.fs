#version 330 core

out vec4 FragColor;
in vec3 ourColor;

void main()
{
    // gl_FragDepth 如果没有在片段着色器中显式设置，着色器会自动设置：gl_FragDepth = gl_FragCoord.z;
    // 输出值 FragColor 如果没有指定，那就是帧缓冲对应附件的默认颜色（可以设置）
    // return是直接返回main函数，return之后的代码无效，但是return之前的代码仍然有效
    // return之前即使没有设置gl_FragDepth的值，着色器仍然会自动设置：gl_FragDepth = gl_FragCoord.z;
    // discard是直接丢弃掉当前片段，即当前片段不会写入对应的缓冲
    return;
    FragColor = vec4(ourColor, 1.0);
    discard;

    gl_FragDepth = gl_FragCoord.z;
}
