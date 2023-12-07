#version 330 core

layout(location = 0) out vec2 depth;
layout(location = 1) out vec4 front_color;
layout(location = 2) out vec4 back_color;

void main ()
{
   depth = vec2(-gl_FragCoord.z, gl_FragCoord.z);
   front_color = vec4(0.f);
   back_color = vec4(0.f);
}
