#version 330 core

layout (location = 0) in vec2 aPos;

uniform float window_height;

void main()
{
   const float point_size = 50.f;

   float y = -1.f + point_size / window_height;

   gl_PointSize = point_size;
   gl_Position = vec4(aPos.x, y, 0., 1.);
}
