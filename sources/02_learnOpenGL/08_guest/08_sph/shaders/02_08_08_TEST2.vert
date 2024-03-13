#version 330 core

layout (location = 0) in vec2 aPos;

void main()
{
   // float x = aPos.x / 8.f;
   // float y = aPos.y / 4.5f;
   // gl_Position = vec4(x, y, 0., 1.);

   gl_Position = vec4(aPos, 0.f, 1.f);
}
