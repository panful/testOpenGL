#version 330 core

out vec4 FragColor;

void main()
{
    float len = length(gl_PointCoord - vec2(.5));

    // 如果当前片段在圆的内部则将该片段设置为指定的颜色
    if(len < 0.5)
    {
        FragColor = vec4(1., 0., 0., 1.);
    }
    else
    {
        discard;
    }
}
