#version 330 core

in float vPathLength;
out vec4 fragColor;

void main() 
{
    float repeat = 15.0;
    float dash = 10.0;
    if (mod(vPathLength, repeat) > dash) {
        discard;
    }
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
