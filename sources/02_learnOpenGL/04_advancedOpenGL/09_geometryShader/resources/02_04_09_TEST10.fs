#version 330 core
out vec4 FragColor;

in vec4 edgeEqn[3];

uniform float lineWidth;
// 边框的颜色
uniform vec3 edgeColor;
// 实体的颜色
uniform vec3 primColor;

void main()
{
    float edist[3];

    edist[0] = dot(edgeEqn[0].xy, gl_FragCoord.xy) + edgeEqn[0].w;
    edist[1] = dot(edgeEqn[1].xy, gl_FragCoord.xy) + edgeEqn[1].w;
    edist[2] = dot(edgeEqn[2].xy, gl_FragCoord.xy) + edgeEqn[2].w;

    // edist小于-0.5表示当前片段在边框外部
    // edgeEqn.z大于0.0表示边框线宽大于0.0

    if (edist[0] < -0.5 && edgeEqn[0].z > 0.0) 
        discard;
    if (edist[1] < -0.5 && edgeEqn[1].z > 0.0) 
        discard;
    if (edist[2] < -0.5 && edgeEqn[2].z > 0.0) 
        discard;

    edist[0] += edgeEqn[0].z;
    edist[1] += edgeEqn[1].z;
    edist[2] += edgeEqn[2].z;

    // 如果是边框emix = 1.0，不是边框emix = 0.0
    float emix = clamp(0.5 + 0.5 * lineWidth - min(min(edist[0], edist[1]), edist[2]), 0.0, 1.0);
    vec3 color = mix(primColor, edgeColor, emix);

    FragColor = vec4(color, 1.0);
}
