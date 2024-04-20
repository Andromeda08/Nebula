#version 460

layout(location = 0) in vec4 i_world_pos;
layout(location = 1) in vec4 i_color;

layout(location = 0) out vec4 o_fragment_color;

void main() {
    o_fragment_color = vec4(i_color.xyz, 0.75);
}