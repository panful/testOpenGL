#version 330 core

out vec4 FragColor;
in vec2 vsOutTexCoord;
uniform sampler2D Texture;

void main()
{    
    FragColor = texture(Texture, vsOutTexCoord);
}