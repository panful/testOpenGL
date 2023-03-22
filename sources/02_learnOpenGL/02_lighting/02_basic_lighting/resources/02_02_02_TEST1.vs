#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

out vec3 Color;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 顶点在世界坐标系的位置
    FragPos = vec3(model * vec4(aPos, 1.0));

    // 顶点法线
    //Normal = aNormal;

    // 对顶点法线也做模型变换，让法线也处于世界坐标系
    // 注意法线只是一个方向，不等比例的缩放会导致法线错误
    // 因此对法线不能仅仅乘一个模型矩阵，而是法线矩阵：模型矩阵左上角3x3部分的逆矩阵的转置矩阵
    // 逆矩阵对于着色器来说开销很大，因为需要对每个顶点进行计算，所以最好是在CPU中计算
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // 顶点颜色
    Color = aColor;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}