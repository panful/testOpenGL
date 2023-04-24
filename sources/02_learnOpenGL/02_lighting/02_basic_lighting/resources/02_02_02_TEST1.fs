#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 lightColor;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(Normal);
    // 光源方向，光源指向物体上顶点的反方向
    vec3 lightDir = normalize(lightPos - FragPos);
    // 法线与光源方向的角度越小，亮度越大 ||A·B|| = ||A|| * ||B|| * cosθ
    // 即法线与光源方向平行时，漫反射分量越大
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec3 result = (ambient + diffuse) * Color;
    FragColor = vec4(result, 1.0);
} 