#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec3 Normal;  
in vec3 FragPos;  

uniform vec3 viewPos;
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

    // specular
    // 镜面强度，越大越亮
    float specularStrength = 0.5;
    // 视线方向向量，眼睛看向物体上顶点的反方向，即物体上的颜色反射到眼睛的方向
    vec3 viewDir = normalize(viewPos - FragPos);
    // 光源经过反射之后的方向
    vec3 reflectDir = reflect(-lightDir, norm);
    // 反光度，越高反射光的能力越强，散射的越少，高光点就会越小
    int shininess = 32;
    // 【光源反射的方向】和【眼睛看向物体的反方向】之间的角度越小，点积结果越大（90°以内）
    // 即当视线方向和光源反射反向平行时，亮度最大
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
            
    vec3 result = (ambient + diffuse + specular) * Color;
    FragColor = vec4(result, 1.0);
} 