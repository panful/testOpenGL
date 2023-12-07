#version 330 core

float near_plane = 0.1f;
float far_plane = 100.f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC [-1,1]
    float d = (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
    return (d - near_plane) / (far_plane - near_plane); // [0,1]
}

void main()
{
    // 将光源在透视投影下的深度值线性化
    gl_FragDepth = LinearizeDepth(gl_FragCoord.z);

    //gl_FragDepth = gl_FragCoord.z;
}