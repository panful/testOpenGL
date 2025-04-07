#version 330

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 vColor;
out float vDistance;

uniform mat4 transform;
uniform vec3 startPos;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);

    if (gl_VertexID % 2 == 0) {
        // 起点
        vDistance = 0.0;
    } else {
        // 终点
        vec3 prevPos = startPos;
        vDistance = distance(aPos, prevPos);
    }

    vColor = aColor;
}
