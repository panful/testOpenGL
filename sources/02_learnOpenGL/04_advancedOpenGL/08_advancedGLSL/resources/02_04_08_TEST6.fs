#version 330 core

out vec4 FragColor;

in _VS_OUT_
{
     vec3 myColor;
}my_fs_in;

void main()
{
     FragColor = vec4(my_fs_in.myColor, 1.0);
}