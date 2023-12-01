#version 330 core

uniform sampler2D min_max_depth;

in vec2 tex_coord;
out vec4 FragColor;

void main()
{
   ivec2 coord = ivec2(gl_FragCoord.xy);
   vec4 color  = texelFetch(min_max_depth, coord, 0);

   FragColor = vec4(-color.r, color.g, 0.f, 1.f);
}
