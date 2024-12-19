/*
* pow(x,y)   返回x的y次方 即 x^y
* abs(x)     返回x的绝对值
* sqrt(x)    返回一个非负实数的平方根 即 sqrt(9) = 3
* dot(x,y)   返回向量x和y的点积  sqrt(dot(p,p)) = length(p)
* cross(x,y) 返回向量x和y的叉积
* length(x)  返回向量x的长度
* mix(x,y,a) 返回 x*(1-a)+y*a
* fract(x)   返回x的小数部分
* mod(x,y)   返回x除以y的余数
* floor(x)   返回x向下取整
* ceil(x)    返回x向上取整
* step(x,y)  如果y小于x返回0，如果y大于等于x返回1
* smoothstep(x,y,z) 当 z < x 返回0，当 z > y 返回1，介于 {x,y} 之间返回平滑的曲线
* min(x,y)   返回 {x,y} 的较小值，可以求并集
* max(x,y)   返回 {x,y} 的较大值，可以求交集
* clamp(x, y, z) = min(max(x, y), z);
* normalize  归一化
* faceforward
* reflect
* refract
* matrixCompMult
* lessThan
* lessThanEqual
* any
*/


/*
0. 片段着色器像素遍历方式
1. 将窗口用十字分成四块
2. pow 窗口显示一个圆（椭圆），窗口是矩形显示的是椭圆
3. abs 窗口中间显示一个小的矩形
4. length 以左下角为圆心，半径为0.5画圆
5. mix 将窗口从左上角到右下角分为两半
6. floor ceil 整个窗口纯绿色或纯红色
7. step 将窗口分成四个颜色块
8. smoothstep
9. 红色十字架 glsl中浮点数比较
10.distance 窗口显示一个圆（椭圆）
*/

#version 330 core
uniform vec2 iResolution;

#define TEST11

#ifdef TEST0

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
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    
    gl_FragColor = vec4(genRGB(uv),1.0);
}

#endif // TEST1

#ifdef TEST2

vec3 genRGB(vec2 uv){
    if(pow(uv.x-0.5,2) + pow(uv.y-0.5,2) < 0.25)
        return vec3(0,1,0);

    return vec3(1,0,0);
}

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    gl_FragColor = vec4(genRGB(uv),1.0);
}

#endif // TEST2

#ifdef TEST3

vec3 genRGB(vec2 uv){
    if(abs(uv.x-0.5)<0.2 && abs(uv.y-0.5)<0.2)
        return vec3(0,1,0);

    return vec3(1,0,0);
}

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    gl_FragColor = vec4(genRGB(uv),1.0);
}

#endif // TEST3

#ifdef TEST4

vec3 genRGB(vec2 uv){
    if(length(uv)>0.5)
        return vec3(0,1,0);

    return vec3(1,0,0);
}

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    gl_FragColor = vec4(genRGB(uv),1.0);
}

#endif // TEST4

#ifdef TEST5

vec3 genRGB(vec2 uv){
    if(mix(uv.x,uv.y,0.5)>0.5)
        return vec3(0,1,0);

    return vec3(1,0,0);
}

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    gl_FragColor = vec4(genRGB(uv),1.0);
}

#endif // TEST5

#ifdef TEST6

vec3 genRGB(vec2 uv){
    //return vec3(floor(uv.x),ceil(uv.y),0.0); // 纯绿色
    //return vec3(ceil(uv.x),floor(uv.y),0.0);   // 纯红色
    //return vec3(ceil(uv),0.0);  // 纯黄色
    return vec3(floor(uv),0.0);   // 纯黑色
}

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    gl_FragColor = vec4(genRGB(uv),1.0);
}

#endif // TEST6

#ifdef TEST7

vec3 genRGB(vec2 uv){
    // 2 和 3 是一样的效果
    // 1 和 4 是一样的效果

    return vec3(step(0.2,uv.x),step(0.8,uv.y),0.0);  // [1]
    //return vec3(step(0.2,uv.x),step(0.2,uv.y),0.0);  // [2]
    //return vec3(step(0.2,uv),0.0); // [3]
    //return vec3(step(vec2(0.2,0.8),uv),0.0); // [4]
}

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    gl_FragColor = vec4(genRGB(uv),1.0);
}

#endif // TEST7

#ifdef TEST8

vec3 genRGB(vec2 uv){
    // 1和2效果一样
    // 4和5效果一样

    //return vec3(smoothstep(0.0,1.0,uv.x),smoothstep(0.0,1.0,uv.y),0.0);  // [1]
    //return vec3(uv.x,uv.y,0.0);  // [2]

    //return vec3(uv/vec2(2,2),0.0);  // [4]
    //return vec3(uv.x/2.0,uv.y/2.0,0.0);  // [5]

    return vec3(smoothstep(0.0,0.5,uv.x),smoothstep(0.0,0.5,uv.y),0.0);  // [7]
    //return vec3(smoothstep(0.5,0.0,uv.x),smoothstep(0.5,0.0,uv.y),0.0);  // [8]
}

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    gl_FragColor = vec4(genRGB(uv),1.0);
}

#endif // TEST8

#ifdef TEST9

vec3 genRGB(vec2 uv){
    if(abs(uv.x - 0.5) < 0.001)
        return vec3(1.0,0.0,0.0);
    if(abs(uv.y - 0.5) < 0.001)
        return vec3(1.0,0.0,0.0);
    return vec3(0.0,0.0,0.0);
}

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    gl_FragColor = vec4(genRGB(uv),1.0);
}

#endif // TEST9

#ifdef TEST10

void main(){
    // 计算所有像素到窗口中心的距离
    float r = distance(gl_FragCoord.xy/iResolution.xy, vec2(0.5, 0.5));
    //根据距离设置片元
    if(r < 0.5){
    // 方形区域片元距离几何中心半径小于0.5，像素颜色设置红色
      gl_FragColor = vec4(1.0,0.0,0.0,1.0);
    }else {
    // 方形区域距离几何中心半径不小于0.5的片元剪裁舍弃掉：
      discard;
    }
}

#endif // TEST10

#ifdef TEST11

void main(){
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    gl_FragColor = vec4(0,1,1,1.0);
}

#endif // TEST11
