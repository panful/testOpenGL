#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

out vec3 fColor;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

mat4 transform;

void GenFront()
{
    fColor = vec3(1.0, 0.0, 0.0);
    float offset = 0.5f;

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, offset, offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, offset, offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, -offset, offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, -offset, offset, 0.f));
    EmitVertex();

    EndPrimitive();
}

void GenBack()
{
    fColor = vec3(0.0, 1.0, 0.0);
    float offset = 0.5f;

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, offset, -offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, offset, -offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, -offset, -offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, -offset, -offset, 0.f));
    EmitVertex();

    EndPrimitive();
}

void GenTop()
{
    fColor = vec3(0.0, 0.0, 1.0);
    float offset = 0.5f;

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, offset, -offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, offset, -offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, offset, offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, offset, offset, 0.f));
    EmitVertex();

    EndPrimitive();
}

void GenBottom()
{
    fColor = vec3(1.0, 1.0, 0.0);
    float offset = 0.5f;

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, -offset, -offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, -offset, -offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, -offset, offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, -offset, offset, 0.f));
    EmitVertex();

    EndPrimitive();
}

void GenRight()
{
    fColor = vec3(0.0, 1.0, 1.0);
    float offset = 0.5f;

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, offset, -offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, offset, offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, -offset, -offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(offset, -offset, offset, 0.f));
    EmitVertex();

    EndPrimitive();
}

void GenLeft()
{
    fColor = vec3(1.0, 0.0, 1.0);
    float offset = 0.5f;

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, offset, -offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, offset, offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, -offset, -offset, 0.f));
    EmitVertex();

    gl_Position = transform * (gl_in[0].gl_Position + vec4(-offset, -offset, offset, 0.f));
    EmitVertex();

    EndPrimitive();
}

void main()
{
    //transform = projection * view * model;
    //transform = model * view * projection;

    //transform = projection;
    transform = model;

    //transform = projection * model ;

    GenFront();
    GenBack();
    GenTop();
    GenBottom();
    GenRight();
    GenLeft();
}