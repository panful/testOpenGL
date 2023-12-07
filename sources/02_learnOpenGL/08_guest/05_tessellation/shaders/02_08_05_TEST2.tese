#version 450 core

layout(triangles, equal_spacing, ccw) in;

in vec4 tcs_pos[];
in vec3 tcs_color[];
  
out vec3 out_Color;
  
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}
 
void main(){
    vec3 iterP = interpolate3D(tcs_pos[0].xyz, tcs_pos[1].xyz, tcs_pos[2].xyz);
    vec3 iterCd = interpolate3D(tcs_color[0], tcs_color[1], tcs_color[2]);
      
    out_Color = iterCd;
    gl_Position = vec4(iterP , 1.0);
}