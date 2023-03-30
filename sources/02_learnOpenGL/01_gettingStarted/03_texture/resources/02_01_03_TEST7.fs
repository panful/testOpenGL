#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D theTexture;

void main()
{
    vec4 tex = texture(theTexture, TexCoord);
    //if(tex.a < 1.)
    //    discard;
    
    FragColor = tex;
}
