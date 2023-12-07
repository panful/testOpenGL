#version 330 core

// https://www.shadertoy.com/view/NscXz2

uniform float iTime;
uniform vec2 iResolution;
uniform sampler3D iChannel0;

out vec4 FragColor;

#define LARGE_NUMBER 1e20
#define MAX_VOLUME_MARCH_STEPS 160  //在Volume中光线步进的最大次数
#define MAX_SDF_DETECT_STEPS 15     //最大探测次数，用于确定物体边界
#define MARCH_STRIDE 0.4            //固定的光线步长（在Volume中）

struct Box{
    vec3 Position;
    vec3 EdgeLength;
};

Box mBox = Box(
    vec3(0,0,0),
    vec3(16.0)
);

struct Camera
{
    vec3 Position;//
    vec3 LookAt;    

    float ImageHeight;      //成像高度//根据屏幕长宽比可求宽度
    float FocalDistance;    //焦距
};
Camera mCamera = Camera(
    vec3(120, 20, -165),
    vec3(0, 0, 0),
    2.0,
    7.0
);

//https://iquilezles.org/articles/distfunctions
float sdBox( vec3 p /*到中心的距离*/, vec3 b/*边长*/ )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float QueryVolumetricDistanceField( in vec3 pos)
{
    float sdfValue= sdBox( pos-  mBox.Position, mBox.EdgeLength);
    return sdfValue;
}

float IntersectRayMarch(in vec3 rayOrigin, in vec3 rayDirection, float maxD)//用于确定物体边界
{
	float precis = MARCH_STRIDE; //这个值太大，会导致边界判断不精确，导致Volume表面有很多曲线形的分界线
    float D = 0.0f;
    for(int i=0; i<MAX_SDF_DETECT_STEPS; i++ )
    {
	    float dis = QueryVolumetricDistanceField( rayOrigin + rayDirection * D);
        if( dis < precis || D > maxD ) 
            break;
        D += dis;
    }
    return  D >= maxD  ? -1.0 : D;//没有碰到物体则返回-1，否则返回深度（据摄像机距离）
}

vec3 CameraOrbit(float speedRatio)//相机在轨道上旋转
{
    float theta = iTime * speedRatio;
    float radius = 165.0;
    
    return vec3(radius * cos(theta), mCamera.Position.y + 50.0 /** sin(theta*4.0)*/, radius * sin(theta));
}

void SetCamera(in vec2 _uv, in float _aspectRatio, out vec3 _rayOrigin, out vec3 _rayDirection)
{
    float ImageWidth = mCamera.ImageHeight * _aspectRatio;
    
    //vec3 ImagePosition = mCamera.Position;
    vec3 ImagePosition = CameraOrbit(0.3);
    vec3 CameraView = mCamera.LookAt - ImagePosition;
    float ViewLength = length(CameraView);
    vec3 CameraViewDir = CameraView / ViewLength;

    vec3 CameraRight = cross(CameraViewDir, vec3(0, 1, 0));    
    vec3 CameraUp = cross(CameraRight, CameraViewDir);

    vec3 focalPoint = ImagePosition - mCamera.FocalDistance * CameraViewDir;//焦点位置
    
    vec3 ImagePoint = ImagePosition;//用Image的中心点初始化成像点
    //根据uv坐标偏移成像点
    ImagePoint += CameraRight * (_uv.x * 2.0 - 1.0) * ImageWidth *.5;
    ImagePoint += CameraUp * (_uv.y * 2.0 - 1.0) * mCamera.ImageHeight *.5;
    
    _rayOrigin = focalPoint;
    _rayDirection = normalize(ImagePoint - focalPoint);
}
vec3 GetAmbientLight()
{
	return vec3(0.03);
}

vec3 Render( in vec3 rayOrigin, in vec3 rayDirection)
{
    //找到volume的边界(若有外部传入的模型，可以直接得到深度。这里的模型是在shader中用代码生成的，所以使用RayMarch的方式确定边界)
    float volumeDepth = IntersectRayMarch(rayOrigin, rayDirection, LARGE_NUMBER);
    
    vec3 volumetricColor = vec3(0.0f);

    //从Volume的边界开始RayMarch
    if(volumeDepth > 0.0)//若可以触碰到Volume(没有被其他物体遮挡或Volume不在这条路径上)
    {
        float signedDistance = .0;
        for(int i = 0; i < MAX_VOLUME_MARCH_STEPS; i++)
        {
            volumeDepth += max(MARCH_STRIDE, signedDistance);//若还没有到达Volume边界，则先步进到边界处
           // volumeDepth += MARCH_STRIDE;
            vec3 position = rayOrigin + volumeDepth * rayDirection;

            signedDistance = QueryVolumetricDistanceField(position);

			if(signedDistance < 0.0f)//在Volume中
            {
                float scale = 32.0*(2.0+sin(iTime));
                vec3 conner = mBox.Position-mBox.EdgeLength/2.0;
                float value = texture(iChannel0, (position-conner)/scale).x;//选取3D纹理的一部分进行采样
                float target =  0.5+0.2 * sin(iTime);
               
                if(value<target||value>target+0.1)
                    value =0.0;
                
                volumetricColor += value * GetAmbientLight();
            }
        }
    }
    return min(volumetricColor, 1.0f);
}


void main()
{
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    
    float aspectRatio = iResolution.x /  iResolution.y;
    
    vec3 rayOrigin, rayDirection;
    
    SetCamera(
        uv, aspectRatio,        //输入
        rayOrigin, rayDirection //输出
    );
        
    vec3 color = Render(rayOrigin, rayDirection);
    
    FragColor = vec4( color, 1.0 );
}