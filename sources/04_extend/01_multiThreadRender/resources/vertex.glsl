#version 430 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vetexColor;
layout(location = 2) in vec2 vertexUV;
uniform mat4 mvp;
out vec4 color;
out vec2 uv;
void main()
{
    gl_Position =mvp*vec4(vertexPos,1); 
    color=vec4(vetexColor,1);
    uv=vertexUV;
}