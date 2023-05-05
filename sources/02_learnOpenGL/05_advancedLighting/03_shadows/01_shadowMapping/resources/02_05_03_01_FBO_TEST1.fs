#version 330 core

out vec4 FragColor;
in vec2 vsOutTexCoords;

uniform sampler2D theTexture;

uniform float near_plane;
uniform float far_plane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{
    // 纹理数据类型：GL_DEPTH_COMPONENT，对应RGBA的R分量
    // G和B始终都是0，A始终都是1
    // FragColor = texture(theTexture, vsOutTexCoords);

    float depthValue = texture(theTexture, vsOutTexCoords).r;
    // 正交投影
    FragColor = vec4(vec3(depthValue), 1.0f);
    // 透视投影
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0);
}