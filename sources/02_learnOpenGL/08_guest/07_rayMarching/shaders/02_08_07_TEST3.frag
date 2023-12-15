#version 330 core

// 体积云范围
uniform vec2 uCloudX;
uniform vec2 uCloudY;
uniform vec2 uCloudZ;

uniform vec3 uCameraPos;

in vec4 worldPos;
out vec4 FragColor;

// 光线步进
vec4 GetCloudColor()
{
    float step = 0.01; // 步长
    int nStep = 1000;  // 步数，步数太小，步长太大，绘制的云会有很明显的梯度变换

    vec3 dir = normalize(worldPos.xyz - uCameraPos);    // 光线步进的方向
    vec3 nextPos = worldPos.xyz;                        // 光线步进的起始点
    vec4 color = vec4(0.f);
    for(int i = 0; i < nStep; ++i)
    {
        nextPos += (step * dir);
        if(nextPos.x < uCloudX.x || nextPos.x > uCloudX.y ||
            nextPos.y < uCloudY.x || nextPos.y > uCloudY.y ||
            nextPos.z < uCloudZ.x || nextPos.z > uCloudZ.y)
        {
            // 如果当前点不在设置的体积云范围内，则跳过
            // 因为从顶点着色器传入片段着色器的世界坐标已经是体积云的边框顶点，所以此处可以使用break
            // 如果存在当前片段不属于体积云，但是光线前进之后可能会是体积云，则应该使用continue
            break;
        }

        vec4 cloud = vec4(.9f, .8f, .7f, 1.f);
        float density = .01; // 云的密度，注意要和步长和步数对应，不能让云的最终颜色大于1.0

        color +=  (cloud * density * (1.f - color.a));
    }

    return color;
}

void main()
{
    vec4 cloudColor = GetCloudColor();
    vec4 bgColor = vec4(0.2f, 0.3f, 0.3f, 1.0f);
    // 将背景和云混合
    FragColor = vec4(bgColor.rgb * (1. - cloudColor.a) + cloudColor.rgb * cloudColor.a, 1.);
}
