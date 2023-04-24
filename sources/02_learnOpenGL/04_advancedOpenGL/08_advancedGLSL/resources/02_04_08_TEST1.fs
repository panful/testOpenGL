#version 330 core

out vec4 FragColor;

in vec3 myColor;

void main()
{
     //FragColor = vec4(myColor, 1.0);

     // 当片段的x小于400时，片段的颜色为红色
     // 当片段的x大于等于400时，片段的颜色为绿色
     if(gl_FragCoord.x < 400)
          FragColor = vec4(1.0, 0.0, 0.0, 1.0);
     else
          FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}