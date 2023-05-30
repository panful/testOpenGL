#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    float d = length(FragPos.xyz - lightPos);   // 顶点到光源的距离
    d = d / far_plane;                             // 将距离归一化到 [0,1]

    gl_FragDepth = d;
}