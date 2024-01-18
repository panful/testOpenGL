#version 330 core

out vec4 FragColor;
in vec3 VSOUT_ViewPos; // 相机空间坐标

void main()
{
    vec3 dx = dFdx(VSOUT_ViewPos);
    vec3 dy = dFdy(VSOUT_ViewPos);

    vec3 normal = normalize(cross(dx, dy)); // 如果当前片段的法线垂直于屏幕，则normal.z = 1，否则normal.z小于1

    float specularPower = 1.f;
    float df = max(0.f, normal.z);
    df = pow(df, specularPower);

    // 与相机方向平行的平行光
    vec3 color = vec3(1.f, 0.f, 0.f);
    color *= df;

    FragColor = vec4(color, 1.f);
} 
