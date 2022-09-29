#version 430 core
in vec4 color;
in vec2 uv;
out vec4 outColor;
layout(binding = 0) uniform sampler2D colorTex1;
layout(binding = 1) uniform sampler2D depthTex1;
layout(binding = 2) uniform sampler2D colorTex2;
layout(binding = 3) uniform sampler2D depthTex2;
void main()
{
    float d1 = texture(depthTex1,uv).r;
    float d2 = texture(depthTex2,uv).r;
    float d = sign(sign(d1-d2)+1);
    outColor = mix(texture(colorTex1,uv),texture(colorTex2,uv),d);
    
}