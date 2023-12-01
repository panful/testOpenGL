#version 330 core

uniform sampler2D Texture;

in vec2 tex_coord;
out vec4 FragColor;

void main()
{
   vec4 color = texture(Texture, tex_coord);
   FragColor = vec4(color.r + .1f, color.g + .1f, color.b + .1f, 1.f);
}
