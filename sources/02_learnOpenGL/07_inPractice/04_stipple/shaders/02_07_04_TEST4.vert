#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in float aPathLength;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out float vPathLength;

void main() 
{
    vPathLength = aPathLength;
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0)
}
