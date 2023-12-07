#version 330 core

out vec4 FragColor;
in vec3 cubeTexCoords;

uniform samplerCube cubeTexture;

void main()
{
    FragColor = texture(cubeTexture, cubeTexCoords);
}
