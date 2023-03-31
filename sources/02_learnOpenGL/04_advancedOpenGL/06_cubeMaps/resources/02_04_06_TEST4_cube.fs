#version 330 core

out vec4 FragColor;

in vec3 vsOutNormal;
in vec3 vsOutPostion;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
    // 摄像机的方向
    vec3 I = normalize(vsOutPostion - cameraPos);
    // 反射向量
    vec3 R = reflect(I, normalize((vsOutNormal)));

    FragColor = vec4(texture(skybox, R));
}
