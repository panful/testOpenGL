#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
   // if(TexCoord.x > 1. || TexCoord.y > 1. || TexCoord.x <= 0. || TexCoord.y <= 0.)
   //    discard;
   FragColor = texture(ourTexture,TexCoord);
}
