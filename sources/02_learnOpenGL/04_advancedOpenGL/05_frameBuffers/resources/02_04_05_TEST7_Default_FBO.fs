#version 330 core

in vec2 vsOutTexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

// 反相
vec4 Inversion()
{
   return vec4(vec3(1.0 - texture(screenTexture, vsOutTexCoords)), 1.0);
}

// 灰度
vec4 Grayscale()
{
   // vec4 _FragColor = texture(screenTexture, vsOutTexCoords);
   // float average = (_FragColor.r + _FragColor.g + _FragColor.b) / 3.0;
   // return vec4(average, average, average, 1.0);

   vec4 _FragColor = texture(screenTexture, vsOutTexCoords);
   float average = 0.2126 * _FragColor.r + 0.7152 * _FragColor.g + 0.0722 * _FragColor.b;
   return vec4(average, average, average, 1.0);
}

// 核效果
vec4 KernelFunc(float kernel[9])
{
   const float offset = 1.0 / 300.0;  

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
      sampleTex[i] = vec3(texture(screenTexture, vsOutTexCoords.st + offsets[i]));
    }

    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
      col += sampleTex[i] * kernel[i];
    }

    return vec4(col, 1.0);
}

void main()
{
   // 锐化
   float sharpen[9] = float[](
      -1, -1, -1,
      -1,  9, -1,
      -1, -1, -1
   );

   // 模糊
   float blur[9] = float[](
      1.0 / 16,   2.0 / 16,   1.0 / 16,
      2.0 / 16,   4.0 / 16,   2.0 / 16,
      1.0 / 16,   2.0 / 16,   1.0 / 16  
   );

   // 边缘检测
   float edge_detection[9] = float [](
      1.0,    1.0,    1.0,
      1.0,   -8.0,    1.0,
      1.0,    1.0,    1.0  
   );

   //FragColor = KernelFunc(blur);
   //FragColor = KernelFunc(sharpen);
   //FragColor = KernelFunc(edge_detection);

   //FragColor = Grayscale();
   FragColor = Inversion();

}
