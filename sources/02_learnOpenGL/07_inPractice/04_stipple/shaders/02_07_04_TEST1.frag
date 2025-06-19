#version 330 core

in vec2 vScreenPos;
out vec4 fragColor;

uniform vec2 uStartPos;

void main() 
{
    float repeat = 15.0;
    float dash = 10.0;
    float length = distance(uStartPos, vScreenPos);
    if (mod(length, repeat) > dash) {
        discard;
    }
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
