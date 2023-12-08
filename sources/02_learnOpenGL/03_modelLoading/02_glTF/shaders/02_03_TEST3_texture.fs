#version 330 core

in vec2 vsOutTexCoord;
uniform sampler2D Texture;

out vec4 FragColor;

void main()
{    
    FragColor = texture(Texture, vsOutTexCoord);
}