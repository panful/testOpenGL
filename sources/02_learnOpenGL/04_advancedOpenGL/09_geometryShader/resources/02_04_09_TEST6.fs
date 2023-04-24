#version 430 core

// gl_Layer 在430以后才支持

out vec4 FragColor;

void main()
{
   if(gl_Layer == 0)
      FragColor = vec4(1.0, 0.0, 0.0, .5);
   else if(gl_Layer == 1)
      FragColor = vec4(0.0, 1.0, 0.0, .5);
   else
      FragColor = vec4(0.0, 0.0, 1.0, .5);
}