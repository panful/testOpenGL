#version 420 core

in vec2 texture_coords;
out vec4 FragColor;

uniform sampler2D screen;

void main()
{
	FragColor = vec4(texture(screen, texture_coords).rgb, 1.0f);
}