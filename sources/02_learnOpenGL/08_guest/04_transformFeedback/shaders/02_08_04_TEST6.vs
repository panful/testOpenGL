#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aVelocity;
layout (location = 2) in float aElasticity;

out vec3 nextPos;
out float nextVelocity;
out float nextElasticity;

uniform float uTime;

void main()
{
    float v = aVelocity;    // 速度
    float t = uTime;        // 时间
    float g = .98;          // 重力加速度，放慢运动
    float h = aPos.y;       // 位置
    float e = aElasticity;  // 弹性系数

    h = h - v * t - 0.5 * g * t * t;
    v = v + g * t;

    // 运动到屏幕最下面时弹起
    if(h < -1.0)
    {
        h = -2.0 - h;
        v = -e * v; 
    }

    nextPos = vec3(aPos.x, h, aPos.z);
    nextVelocity = v;
    nextElasticity = e;

    gl_Position = vec4(aPos, 1.0);
}