#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

out vec3 fColor;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

mat4 transform;

// 立方体的八个顶点向量
vec4 BackTopRight =  vec4( 1,  1, 1, 1);
vec4 BackTopLeft =     vec4(-1,  1, 1, 1);
vec4 BackBottomLeft =  vec4(-1, -1, 1, 1);
vec4 BackBottomRight = vec4( 1, -1, 1, 1);

vec4 FrontTopRight =    vec4( 1,  1, -1, 1);
vec4 FrontTopLeft =     vec4(-1,  1, -1, 1);
vec4 FrontBottomLeft =  vec4(-1, -1, -1, 1);
vec4 FrontBottomRight = vec4( 1, -1, -1, 1);

// 生成立方体6个面
void GenFront()
{
    fColor = vec3(1.0, 0.0, 0.0);

    gl_Position = transform * FrontBottomLeft;
    EmitVertex();

    gl_Position = transform * FrontBottomRight;
    EmitVertex();

    gl_Position = transform * FrontTopLeft;
    EmitVertex();

    gl_Position = transform * FrontTopRight;
    EmitVertex();

    EndPrimitive();
}

void GenBack()
{
    fColor = vec3(0.0, 1.0, 0.0);

    gl_Position = transform * BackBottomRight;
    EmitVertex();

    gl_Position = transform * BackBottomLeft;
    EmitVertex();

    gl_Position = transform * BackTopRight;
    EmitVertex();

    gl_Position = transform * BackTopLeft;
    EmitVertex();

    EndPrimitive();
}

void GenTop()
{
    fColor = vec3(0.0, 0.0, 1.0);

    gl_Position = transform * FrontTopLeft;
    EmitVertex();

    gl_Position = transform * FrontTopRight;
    EmitVertex();

    gl_Position = transform * BackTopLeft;
    EmitVertex();

    gl_Position = transform * BackTopRight;
    EmitVertex();

    EndPrimitive();
}

void GenBottom()
{
    fColor = vec3(1.0, 1.0, 0.0);

    gl_Position = transform * FrontBottomLeft;
    EmitVertex();

    gl_Position = transform * FrontBottomRight;
    EmitVertex();

    gl_Position = transform * BackBottomLeft;
    EmitVertex();

    gl_Position = transform * BackBottomRight;
    EmitVertex();

    EndPrimitive();
}

void GenRight()
{
    fColor = vec3(0.0, 1.0, 1.0);

    gl_Position = transform * FrontBottomRight;
    EmitVertex();

    gl_Position = transform * BackBottomRight;
    EmitVertex();

    gl_Position = transform * FrontTopRight;
    EmitVertex();

    gl_Position = transform * BackTopRight;
    EmitVertex();

    EndPrimitive();
}

void GenLeft()
{
    fColor = vec3(1.0, 0.0, 1.0);
    float offset = 0.5f;

    gl_Position = transform * BackBottomLeft;
    EmitVertex();

    gl_Position = transform * FrontBottomLeft;
    EmitVertex();

    gl_Position = transform * BackTopLeft;
    EmitVertex();

    gl_Position = transform * FrontTopLeft;
    EmitVertex();

    EndPrimitive();
}

void main()
{
    transform = projection * view * model;

    vec4 originPos = gl_in[0].gl_Position;

    BackTopRight        += originPos;
    BackTopLeft         += originPos;
    BackBottomLeft      += originPos;
    BackBottomRight     += originPos;
    FrontTopRight       += originPos;
    FrontTopLeft        += originPos;
    FrontBottomLeft     += originPos;
    FrontBottomRight    += originPos;

    GenFront();
    GenBack();
    GenTop();
    GenBottom();
    GenRight();
    GenLeft();
}
