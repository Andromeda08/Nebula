#version 460
#extension GL_EXT_mesh_shader : require

layout (location = 0) in MeshIn {
    vec3 color;
} m_in;
layout (location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(m_in.color, 1.0);
}