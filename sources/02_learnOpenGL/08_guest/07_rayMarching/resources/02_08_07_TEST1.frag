#version 330 core

// 三维坐标：左手坐标系,X轴指向屏幕右侧，Y轴指向屏幕上面，Z轴垂直于屏幕向里

// 最大步数
// 最大距离
// 最小步进的距离
#define Max_Steps 100
#define Max_Dist 100.
#define Surf_Dist 0.01

uniform float iTime;
uniform vec2 iResolution;

out vec4 FragColor;

vec4 spherePosRadius = vec4(0, 1, 6, 1); // 球的位置(xyz)和半径（w）
vec3 lightPos = vec3(0, 5, 6);

// 求点到球或平面的最近距离
float GetDist(vec3 p)
{
    vec4 s = spherePosRadius;
    float sphereDist = length(p-s.xyz)-s.w; // P点到球面的距离
    float planeDist  = p.y;                 // P点到平面的距离
    
    float d = min(sphereDist, planeDist);
    return d;
}

// 光线步进
float RayMarch(vec3 ro, vec3 rd)
{
    float d0 = 0.;
    for(int i = 0; i < Max_Steps; i++)
    {
        vec3 p = ro + rd*d0;
        float ds = GetDist(p);
        d0 += ds;
        if(d0 > Max_Dist || ds < Surf_Dist) 
        {
            break;
        }      
    }
                     
    return d0;     
}

// 顶点的法线
vec3 GetNormal(vec3 p)
{
    float d = GetDist(p);
    vec2 e = vec2(0.001, 0);
    
    // 分别对xyz分量求偏导数即为法线
    float fdx = d - GetDist(p - e.xyy);
    float fdy = d - GetDist(p - e.yxy);
    float fdz = d - GetDist(p - e.yyx);
    return normalize(vec3(fdx, fdy, fdz));
}

// 计算灯光，点光源
float GetLight(vec3 p)
{
    lightPos.xz += vec2(sin(iTime),cos(iTime))*2.0;

    vec3 l = normalize(lightPos - p);
    vec3 n = GetNormal(p);
    
    float dif = clamp(dot(n,l), 0.0, 1.0);
    
    // 计算阴影
    // p + n * 0.02相当于将模型扩大了一圈
    // l是光线的反方向（从片段指向光源）
    // d就相当于是当前片段（加了一个误差）沿着光线方向与模型的交点距离
    // 如果没有交点，d肯定大于光源到p点的距离，也就意味着不在阴影中
    float d = RayMarch(p + n * 0.02, l);
    if(d < length(lightPos - p))
    {
        // 这个片段在阴影中
        dif *= 0.1;
    }

    return dif;
}

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 uv = (fragCoord - 0.5*iResolution.xy)/iResolution.xy; // [-0.5, 0.5]
    vec3 col = vec3(0);
    
    // Camera：ro 位置， rd 方向
    vec3 ro = vec3(0, 1, 0);
    vec3 rd = normalize(vec3(uv.x, uv.y, 1.0));
    
    float d = RayMarch(ro, rd);
    
    // p点就是摄像机射线与图元的交点，如果没有交点则p是一个很大的值
    vec3 p = ro + rd*d;
    
    float dif = GetLight(p);
    
    col = vec3(dif);

    // Output to screen
    FragColor = vec4(col, 1.0);
}
