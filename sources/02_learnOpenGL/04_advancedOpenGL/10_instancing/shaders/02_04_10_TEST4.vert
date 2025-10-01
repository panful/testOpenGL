#version 330 core

layout (location = 0) in vec3 aPos;        // 四棱锥局部顶点
layout (location = 1) in vec3 iPos;        // instance: 位置
layout (location = 2) in vec4 iQuat;       // instance: 四元数

uniform mat4 viewProj;

vec3 quatRotate(vec4 q, vec3 v) {
    // q = (x,y,z,w)
    vec3 t = 2.0 * cross(q.xyz, v);
    return v + q.w * t + cross(q.xyz, t);
}

void main() {
    vec3 worldPos = quatRotate(iQuat, aPos) + iPos;
    gl_Position = viewProj * vec4(worldPos, 1.0);
}
