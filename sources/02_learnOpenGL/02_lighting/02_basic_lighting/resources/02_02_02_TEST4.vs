#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;
uniform vec3 lightPos; 
uniform vec3 lightColor;

out vec3 Color;

void main()
{
    // 顶点在世界坐标系的位置
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));

    // 顶点法线
    //Normal = aNormal;

    // 对顶点法线也做模型变换，让法线也处于世界坐标系
    // 注意法线只是一个方向，不等比例的缩放会导致法线错误
    // 因此对法线不能仅仅乘一个模型矩阵，而是法线矩阵：模型矩阵左上角3x3部分的逆矩阵的转置矩阵
    // 逆矩阵对于着色器来说开销很大，因为需要对每个顶点进行计算，所以最好是在CPU中计算
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;

    //-----------------------------------------------------------
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = .5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    int shininess = 128;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 resultColor = (ambient + diffuse + specular) * aColor;
    //-------------------------------------------------------------

    // 顶点颜色
    Color = resultColor;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}