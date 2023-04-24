#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 vsOutNormal;
out vec3 vsOutPostion;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 世界空间中的法线向量
    vsOutNormal = mat3(transpose(inverse(model))) * aNormal;
    // 世界空间的位置向量
    vsOutPostion = vec3(model * vec4(aPos, 1.0));

    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
