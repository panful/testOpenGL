#version 330 core

uniform vec4 uColor;
out vec4 FragColor;

void main()
{
    FragColor = uColor;
    gl_FragDepth = gl_FragCoord.z;
}

