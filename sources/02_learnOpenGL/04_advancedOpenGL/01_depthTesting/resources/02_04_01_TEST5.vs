#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 transform;

void main()
{
    vec4 thePos = transform * vec4(aPos, 1.0f);

    //gl_Position = thePos;

    // 顶点着色器之后会进行透视除法，在这里将顶点的z值设置为w，做完透视除法之后，该顶点的深度值就会成为1.0
    // 因为调用完glClear(GL_DEPTH_BUFFER_BIT)之后，深度缓冲中的所有值都为1.0
    // 且当前深度测试函数为GL_LESS，所以当深度值为1.0时，片段不会通过深度测试
    // 如果硬件支持提前深度测试，那么该顶点在未进入片段着色器之前就会被直接丢弃
    gl_Position = vec4(thePos.xyww);
}
