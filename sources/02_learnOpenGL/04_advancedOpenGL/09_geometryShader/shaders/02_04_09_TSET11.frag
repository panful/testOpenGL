#version 330 core

uniform sampler2D tex;

in vec2 fragUV;
out vec4 FragColor;

void main()
{
   vec4 color = texture(tex, fragUV);
   FragColor = color;
}
