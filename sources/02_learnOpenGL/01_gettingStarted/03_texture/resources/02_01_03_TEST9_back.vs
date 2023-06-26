#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
   vec4 pos = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   // 将z设置为w，这样经过透视除法之后，深度值就为最大值，让背景可以在最下面
   gl_Position = pos.xyww;
   TexCoord = aTexCoord;
}
