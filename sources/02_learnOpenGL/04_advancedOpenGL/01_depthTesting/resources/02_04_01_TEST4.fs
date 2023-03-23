#version 330 core

out vec4 FragColor;
in vec3 ourColor;

// 深度值是一个 [0,1] 之间的float值，即 gl_FragCoord.z ∈ [0,1]

void main()
{
    // 透视投影，从红色到绿色的转变不是匀速的，即不是线性的
    // 而且红色持续的时间很短，因为z值越小，深度值变化越快
    // 正交投影红色到绿色的转变就是匀速的
    if(true)
    {
        if(gl_FragCoord.z < 0.5)
            FragColor = vec4(1.f, 0.f, 0.f, 1.f);
        else
            FragColor = vec4(0.f, 1.f, 0.f, 1.f);
    }

    // 将屏幕空间中非线性的深度值变换至线性深度值
    if(false)
    {
        // 近平面为0.1，远平面为100.0
        float near = .1f;
        float far = 100.f;
        
        // 将深度值从 [0, 1] 转换到 [-1, 1]
        float z = gl_FragCoord.z * 2. - 1.;
        // 将透视投影得到的非线性深度值转换为线性深度值
        float linearDepth = (2. * near * far) / (far + near - z * (far - near));
        // 将线性的深度值归一化到 [0,1]
        float normDepth = (linearDepth - near) / (far - near);

        if(normDepth < 0.05)
            FragColor = vec4(1.f, 0.f, 0.f, 1.f);
        else
            FragColor = vec4(0.f, 1.f, 0.f, 1.f);
    }

}


