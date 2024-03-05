#version 460

struct Mesh {
    vec4 color;
};

layout (location = 0) in Mesh IN;
layout (location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(IN.color.xyz, 1.0);
}