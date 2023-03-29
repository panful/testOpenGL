#version 330 core

in vec2 vsOutTexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
   FragColor = vec4(texture(screenTexture, vsOutTexCoords).rgb, 1.0);
}
