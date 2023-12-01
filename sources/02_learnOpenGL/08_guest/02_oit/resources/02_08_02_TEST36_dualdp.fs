#version 330 core

layout(location = 0) out vec2 depth;
layout(location = 1) out vec4 front_color;
layout(location = 2) out vec4 back_color;

uniform sampler2D depth_tex;

uniform vec4 uColor;

void main ()
{
   ivec2 pixelCoord  = ivec2(gl_FragCoord.xy);

   vec2 lastDepth    = texelFetch(depth_tex, pixelCoord, 0).xy;
   float front_depth = -lastDepth.x;
   float back_depth  = lastDepth.y;

   gl_FragDepth = gl_FragCoord.z;

   // 将以下三个值全部设置为默认的最小值，所以符合条件的片段（深度值，颜色值）就是最大的
   // 因为GL_BLEND的计算方法为GL_MAX，因此就会保留
   front_color = vec4(0.f);
   back_color  = vec4(0.f);
   depth       = vec2(-1.f);

   float epsilon = 0.0000001;

   // 最大最小深度值区间之外的片段
   if(gl_FragDepth  < front_depth - epsilon || gl_FragDepth > back_depth + epsilon)
   {
      return;
   }

   // 最大最小值区间之间
   if(gl_FragDepth > front_depth + epsilon && gl_FragDepth < back_depth - epsilon)
   {
      depth = vec2(-gl_FragCoord.z, gl_FragCoord.z);
      return;
   }

   // 初始化的默认值为vec4(0.f) 所以只要符合最大或最小值(颜色值肯定不小于0)就会被保留
   if(gl_FragDepth <= front_depth + epsilon && gl_FragDepth >= front_depth - epsilon) // 最小值
   {
      front_color = uColor;
   }
   else // 最大值
   {
      back_color = uColor;
   }
}
