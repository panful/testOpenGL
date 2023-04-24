#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 cubeTexCoords;

uniform mat4 transform;

void main()
{
    vec4 pos = transform * vec4(aPos, 1.0f);
    // 顶点着色器之后会进行透视除法，在这里将顶点的z值设置为w
    // 做完透视除法之后，该顶点的深度值就会成为1
    // @note 深度缓冲中数据的范围为：[0, 1]
    gl_Position = vec4(pos.xyww);
    cubeTexCoords = aPos;
}
