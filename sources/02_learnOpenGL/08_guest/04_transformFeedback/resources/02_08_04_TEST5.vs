#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 nextPos;

uniform float uTime;
uniform float uSpeed;

void main()
{
    float x = aPos.x;
    float y = uTime * uSpeed + aPos.y;

    y = y > 0.8 ? -0.8 : y;
    nextPos = vec3(x, y, 0.0);

    gl_Position = vec4(aPos, 1.0);
}