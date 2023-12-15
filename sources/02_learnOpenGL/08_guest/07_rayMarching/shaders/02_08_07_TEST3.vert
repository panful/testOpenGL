#version 330 core

layout (location = 0) in vec3 inPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 worldPos;

void main()
{
   worldPos = model * vec4(inPos, 1.f);
   gl_Position = proj * view * worldPos;
}