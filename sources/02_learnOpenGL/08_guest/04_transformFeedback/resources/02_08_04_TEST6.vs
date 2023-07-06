#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aVelocity;

out vec3 nextPos;
out float nextVelocity;

uniform float uTime;

void main()
{
    float x = aPos.x;
    float y = aPos.y;
    float z = 0.0;
    float v = aVelocity;

    y = y - v * uTime;

    if(y < -0.8)
    {
        y = (-1.6 - y);
        v = -0.5 * v;
    }

    if(y > 0.8)
    {
        y = 0.8 - (y - 0.8);
        v = -2 * v;
    }

    if(v > 0.0)
    {
        v = v + 0.01 * v;
    }
    else if(v < 0.0)
    {
        v = v - 0.01 * v;
    }

    nextPos = vec3(x, y, z);
    nextVelocity = v;

    gl_Position = vec4(aPos, 1.0);
}