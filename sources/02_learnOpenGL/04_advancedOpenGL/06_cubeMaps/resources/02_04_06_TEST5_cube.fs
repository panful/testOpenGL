#version 330 core

out vec4 FragColor;

in vec3 vsOutNormal;
in vec3 vsOutPostion;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
    // 折射率
    float ratio = 1.0 / 1.52;

    // 摄像机的方向
    vec3 I = normalize(vsOutPostion - cameraPos);
    // 折射向量
    vec3 R = refract(I, normalize((vsOutNormal)), ratio);

    FragColor = vec4(texture(skybox, R));
}
