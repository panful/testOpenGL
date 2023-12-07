#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

flat out int vertexIDVSOutput;

void main()
{
    vertexIDVSOutput = gl_VertexID;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
