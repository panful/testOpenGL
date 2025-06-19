#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform vec2 uResolution;

out vec2 vScreenPos;

void main() 
{
    vec3 worldPos = vec3(uModel * vec4(aPos, 1.0));
    vec4 clipPos = uProj * uView * vec4(worldPos, 1.0);

    vScreenPos = (clipPos.xy / clipPos.w + 1.0) * 0.5 * uResolution;

    gl_Position = clipPos;
}
