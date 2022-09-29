/*
uniform vec3      iResolution;           // viewport resolution (in pixels)    视口分辨率（像素）
uniform float     iTime;                 // shader playback time (in seconds)  着色器播放时间（秒）
uniform float     iTimeDelta;            // render time (in seconds)           渲染时间（秒）
uniform int       iFrame;                // shader playback frame              着色器播放帧
uniform float     iChannelTime[4];       // channel playback time (in seconds) 通道播放时间（秒）
uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)     通道分辨率（像素）
uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click  鼠标像素坐标
uniform sampler2D iChannel0;             // input channel. XX = 2D/Cube
uniform sampler2D iChannel1;             // input channel. XX = 2D/Cube
uniform sampler2D iChannel2;             // input channel. XX = 2D/Cube
uniform sampler2D iChannel3;             // input channel. XX = 2D/Cube
uniform vec4      iDate;                 // (year, month, day, time in seconds)
uniform float     iSampleRate;           // sound sample rate (i.e., 44100)    声音采样率（即44100）
*/

/*
0. 最基本使用方法
1. 红色椭圆
2. 红色爱心
3. 红色爱心
4. 圆（准确来说是椭圆）
5. 一个简单的圆 https://www.shadertoy.com/view/XsjGDt
6. 随时间变化的渐变图
7. https://www.zhihu.com/question/23115824/answer/2400640371?utm_source=wechat_session&utm_medium=social&utm_oi=777527289679200256&utm_content=group2_Answer&utm_campaign=shareopn
8. 随机数
9. TEST7的其他例子
10 SDF smin https://zhuanlan.zhihu.com/p/491686813
11 会动的云 https://juejin.cn/post/7123419366263095309
*/

#version 330 core
#define TEST11

#ifdef TEST0

uniform vec2 iResolution;

vec3 genRGB(vec2 uv){
    //return vec3(uv.x,0,0);
    //return vec3(0,uv.y,0);
    return vec3(uv.x,uv.y,0);
}

void main() {
    // 从左下角开始依次遍历每一个像素值
    vec2 uv = gl_FragCoord.xy / iResolution.xy; //归一化uv坐标[0,1]
    
    gl_FragColor = vec4(genRGB(uv),1.0);
}

#endif // TEST0

#ifdef TEST1

uniform vec2 iResolution;

vec3 genRGB(vec2 uv){
    float _r = 0.0;
    float _g = 0.0;
    float _b = 0.0;

    if(uv.x>0.5)
        _r = 1.0;
    if(uv.y>0.5)
        _g = 1.0;

    return vec3(_r,_g,_b);
}

void main() {
    // 从左下角开始依次遍历每一个像素值
    vec2 uv = gl_FragCoord.xy / iResolution.xy; //归一化uv坐标[0,1]
    
    gl_FragColor = vec4(genRGB(uv),1.0);
}

#endif // TEST1

#ifdef TEST2

uniform vec2 iResolution;
const vec3 white = vec3(1.);
const vec3 red = vec3(0.9608, 0.2745, 0.0275);

vec3 sdfHeart2(vec2 uv){
    float value = pow(length(uv)-0.2,3.);
    float d = value - pow(uv.x,2.)*pow(uv.y,3.);
    return d > 0.? white: red;
}
void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy; //归一化uv坐标[0,1]
    uv -= 0.5;
    gl_FragColor = vec4(sdfHeart2(uv),1.0);
}

#endif  // TEST2

#ifdef TEST3

uniform vec2 iResolution;
const vec3 white = vec3(1.);
const vec3 red = vec3(0.9608, 0.2745, 0.0275);

float inHeart(vec2 uv, vec2 center)
{
    vec2 o = (uv - center) / (0.5);
    float a = o.x * o.x + o.y * o.y - 0.3;
    float s0 = a * a * a;
    float s1 = o.x * o.x * o.y * o.y * o.y;
    return step(s0, s1);
}

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    gl_FragColor = vec4(mix(white,red,inHeart(uv, vec2(0.5))),1.);
}

#endif // TEST3

#ifdef TEST4

uniform vec2 iResolution;
const vec3 white = vec3(1.);
const vec3 red = vec3(0.9608, 0.2745, 0.0275);

vec3 sdfHeart2(vec2 uv){
    return (pow(uv.x,2) + pow(uv.y,2))>0.25?white:red;  //因为uv归一化后在[-0.5,0.5]之间，所以此处要小于0.25
}
void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy; //归一化uv坐标[0,1]
    uv -= 0.5;
    gl_FragColor = vec4(sdfHeart2(uv),1.0);
}
#endif // TEST4

#ifdef TEST5

uniform vec2 iResolution;
vec3 rgb(float r, float g, float b) {
    return vec3(r / 255.0, g / 255.0, b / 255.0);
}

vec4 circle(vec2 uv, vec2 pos, float rad, vec3 color) {
    float d = length(pos - uv) - rad;
    float t = clamp(d, 0.0, 1.0);
    return vec4(color, 1.0 - t);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {

    vec2 uv = fragCoord.xy;
    vec2 center = iResolution.xy * 0.5;
    float radius = 0.25 * iResolution.y;

    // Background layer
    vec4 layer1 = vec4(rgb(210.0, 222.0, 228.0), 1.0);
    
    // Circle
    vec3 red = rgb(225.0, 95.0, 60.0);
    vec4 layer2 = circle(uv, center, radius, red);
    
    // Blend the two
    // mix返回线性混合的x和y  mix(x,y,a){x*(1-a) + y*a}
    fragColor = mix(layer1, layer2, layer2.a);
}

void main(void)
{
    vec4 fColor;
    mainImage(fColor,gl_FragCoord.xy);
    gl_FragColor = fColor;
}

#endif // TEST5

#ifdef TEST6

uniform vec2      iResolution;
uniform float     iTime;

void main() {
    vec2 st = gl_FragCoord.xy/iResolution.xy;
    st.x *= iResolution.x/iResolution.y;

    vec3 color = vec3(0.);
    color = vec3(st.x,st.y,abs(sin(iTime)));

    gl_FragColor = vec4(color,1.0);
}

#endif // TEST6

#ifdef TEST7

uniform vec2      iResolution;
out vec4          outFragColor;

void main()
{
    float i = gl_FragCoord.x;
    float j = gl_FragCoord.y;
    float x_2 = iResolution.x/2;
    float y_2 = iResolution.y/2;

    float r = pow(cos(atan(j-y_2,i-x_2)/2),2);
    float g = pow(cos(atan(j-y_2,i-x_2)/2-2*acos(-1.)/3),2);  // acos(-1) = 3.1415926
    float b = pow(cos(atan(j-y_2,i-x_2)/2+2*acos(-1.)/3),2);

    outFragColor = vec4(r,g,b,1.0);  // gl_FragColor opengl1.2之后不推荐使用
    // gl_FragColor 移除的原因 https://qa.1r1g.com/sf/ask/4456244831/
}

#endif // TEST7

#ifdef TEST8

uniform vec2      iResolution;
out vec4          outFragColor;

// 随机数 https://jishuin.proginn.com/p/763bfbd73361
float random (vec2 st) {
    return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123);
}

void main()
{
    vec2 uv = gl_FragCoord.xy/iResolution.xy;

    outFragColor = vec4(vec3(random(uv)),1.0);
}

#endif // TEST8


#ifdef TEST9

uniform vec2      iResolution;
out vec4          outFragColor;

float genR(float i,float j)
{
    float s = 3. / (j + 60.0f);
    float y = (j + sin((i * i + pow((j - 420.0f),2.0f) * 5.0f) / 60.0f / 1204.0f) * 20.0f) * s;

    float a = mod(int((i+1024.0f)*s+y),2);
    float b = mod(int((1024.0f * 2 - i) * s + y),2);

    return (a+b) * 0.5f;
}

float genG(float i,float j)
{
    float s = 3. / (j + 60.0f);
    float y = (j + sin((i * i + pow((j - 420.0f),2.0f) * 5.0f) / 60.0f / 1204.0f) * 20.0f) * s;

    float a = mod(int(5 * ((i + 1024.0f) * s + y)), 2);
    float b = mod(int(5 * ((1024.0f * 2 - i) * s + y)), 2);

    return (a+b) * 0.5f;
}

float genB(float i,float j)
{
    float s = 3. / (j + 60.0f);
    float y = (j + sin((i * i + pow((j - 420.0f),2.0f) * 5.0f) / 60.0f / 1204.0f) * 20.0f) * s;

    float a = mod(int(29.0f * ((i + 1024.0f) * s + y)), 2);
    float b = mod(int(29.0f * ((1024.0f * 2.0f - i) * s + y)), 2);

    return (a+b) * 0.5f;
}

void main()
{
    float x = gl_FragCoord.x;
    float y = gl_FragCoord.y;

    float r = genR(x,y);
    float g = genG(x,y);
    float b = genB(x,y);

    outFragColor = vec4(r,g,b,1.0);
}

#endif // TEST9

#ifdef TEST10

uniform vec2      iResolution;
out vec4          outFragColor;

float sdCircle( vec2 p, float r )
{
    return length(p) - r;
}

float Scene(vec2 uv){
    float w = 1.0/iResolution.y;
    float d= sdCircle(uv-vec2(0.2,0.2),0.3);
    
   float c = sdCircle(uv-vec2(0.0,0.0),0.3);
    
    //d = min(-d,c);
    d = max(d,c);
    float r= smoothstep(w,-w, d);
    
    return r;
}

void main()
{
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    uv-=0.5;
    uv.x*=iResolution.x/iResolution.y;
    vec3 col = vec3(0.0);
    
    col = mix(col,vec3(1.0,0.0,1.0),Scene(uv));

    outFragColor = vec4(col,1.0);
}

#endif // TEST10

#ifdef TEST11

precision mediump float;
uniform vec2 iResolution;
uniform float iTime;

float circle(vec2 uv, vec2 center, float d) {
    return length(uv - center) - d;
}

float cloud(vec2 uv, vec2 center, float d) {
    float step = 1.2;
    float c1 = circle(uv, vec2(center.x - d * 0.9 * 1.0 * step, center.y), d * 0.9);
    float c2 = circle(uv, vec2(center.x - d * 0.8 * 2.0 * step, center.y), d * 0.8);
    float c3 = circle(uv, vec2(center.x, center.y), d);

    float c4 = circle(uv, vec2(center.x + d * 0.9 * 1.0 * step, center.y), d * 0.9);
    float c5 = circle(uv, vec2(center.x + d * 0.8 * 2.0 * step, center.y), d * 0.8);
    return min(c5, min(c4, min(c3, min(c1, c2))));
}

void main () {
    vec2 uv = gl_FragCoord.xy / iResolution;

    //+++++++++++++++
    // 让云动起来
    uv.x += 0.2*iTime;  // 0.2对应的参数越大，云飘动的越快
    uv.x = fract(uv.x); // 取小数部分
    //+++++++++++++++

    //+++++++++++++++
    // 添加背景
    vec3 backCol = vec3(0.2078, 0.7765, 1.0);
    vec3 col = backCol;
    float d = cloud(uv, vec2(0.5, 0.5), 0.05);
    d = 1.0 - step(0.0, d);
    //+++++++++++++++

    vec3 cloudCol = vec3(d);
    col = mix(col, cloudCol, d);
    
    gl_FragColor = vec4(col, 1.0);
}

#endif // TEST11