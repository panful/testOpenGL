#version 330 core
out vec4 FragColor;

// 每一个单元的颜色（点、线、三角面）
// 可以通过uniform设置，也可以通过TBO设置
vec4 colors[8] = vec4[8]
(
    vec4(1., 0., 0., 1.),
    vec4(1., 1., 0., 1.),
    vec4(0., 1., 0., 1.),
    vec4(0., 1., 1., 1.),
    vec4(1., 0., 0., 1.),
    vec4(1., 1., 0., 1.),
    vec4(0., 1., 0., 1.),
    vec4(0., 1., 1., 1.)
);

void main()
{
    //gl_PrimitiveID是每一个单元的ID，从0开始
    FragColor = colors[gl_PrimitiveID];
}
