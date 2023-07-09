#version 330 core
uniform sampler2D sprite_texture;
out vec4 FragColor;

void main()
{
    FragColor = texture(sprite_texture, gl_PointCoord);
}