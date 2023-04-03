#version 420 core // 注意GLSL的版本

out vec4 FragColor;
in vec3 myColor;

// any	     默认值。提前深度测试是禁用的，你会损失很多性能
// greater	你只能让深度值比gl_FragCoord.z更大
// less	     你只能让深度值比gl_FragCoord.z更小
// unchanged	如果你要写入gl_FragDepth，你将只能写入gl_FragCoord.z的值

layout (depth_greater) out float gl_FragDepth;

// 通过将深度条件设置为greater或者less，OpenGL就能假设你只会写入比当前片段深度值更大或者更小的值了。
// 这样子的话，当深度值不符合深度测试函数时，OpenGL仍是能够进行提前深度测试的。

void main()
{             
     FragColor = vec4(myColor, 1.0);
     
     // 将深度值统一加0.2
     gl_FragDepth = gl_FragCoord.z + 0.2;
}  