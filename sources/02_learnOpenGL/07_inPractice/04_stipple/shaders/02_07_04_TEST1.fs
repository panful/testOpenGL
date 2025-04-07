#version 330 core

in float vDistance;
in vec3 vColor;

out vec4 fColor;

void main()
{
    float dashSize = 0.1f;
    float gapSize = 0.1f;
    float cycle = dashSize + gapSize;

    float posInCycle = mod(vDistance, cycle);
    if (posInCycle > dashSize) discard;

    fColor = vec4(vColor, 1.0);
}
