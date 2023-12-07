#version 330 core

flat in int vertexIDVSOutput;
out vec4 FragColor;

uniform float cOffset;

void main()
{
    gl_FragDepth = gl_FragCoord.z + cOffset/ 65000. ;

    int idx = vertexIDVSOutput+1;
    FragColor = vec4(float(idx%256)/255.0, float((idx/256)%256)/255.0, float((idx/65536)%256)/255.0, 1.0);
}
