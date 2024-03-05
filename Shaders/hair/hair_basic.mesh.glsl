#version 460
#extension GL_EXT_mesh_shader : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int8 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#define WORKGROUP_SIZE 32

layout (local_size_x = WORKGROUP_SIZE) in;
layout (triangles, max_vertices = 96, max_primitives = 84) out;

// Descriptors, Push Constants
layout (push_constant) uniform HairConstants {
    mat4     model;
    ivec4    buffer_lengths;        // [ VTX, SDESC, -, - ]
    uint64_t vertex_address;
    uint64_t strand_desc_address;
} push;

layout (binding = 0) uniform CameraData {
    mat4 view;
    mat4 proj;
    mat4 view_inverse;
    mat4 proj_inverse;
    vec4 eye;
} camera;

// Buffer references
struct Vertex {
  vec4 position;
};
layout (buffer_reference, scalar) buffer Vertices {
    Vertex vertices[];
};
layout (buffer_reference, scalar) buffer StrandDescriptions {
    ivec4 strand_descs[];
};

// [Mesh] Output parameters
layout (location = 0) out MeshOut {
    vec3 color;
} m_out[];

void main()
{
    if (gl_WorkGroupID.x == 0 && gl_LocalInvocationID.x == 0) {
        debugPrintfEXT("Hi from Mesh WG #0 & Lane #0\n");
    }

    uint ID = gl_LocalInvocationID.x;
    if (ID > 25) {
        return;
    }

    uint i_offset	= ID * 2;
    uint v_offset	= ID * 4;
    uint quads 	    = 25 - 1;
    uint vtx_count  = quads * 4;
    uint tri_count  = quads * 2;

    SetMeshOutputsEXT(vtx_count, tri_count);

    Vertices v = Vertices(push.vertex_address);

    const uint work_group	= gl_WorkGroupID.x;
    const uint vb_offset 	= (work_group * 25) + ID;

    // A - D    0 - 3
    // | / |    | / |
    // B - C    1 - 2
    // 2,1,0 & 3,2,0
    const vec4 thickness = vec4(1, 0, 0, 0); // push.offset.xyz;
    Vertex generated[4];
    generated[0] = v.vertices[vb_offset + 0];

    generated[1] = v.vertices[vb_offset + 0];
    generated[1].position += thickness;

    generated[2] = v.vertices[vb_offset + 1];
    generated[2].position += thickness;

    generated[3] = v.vertices[vb_offset + 1];

    const mat4 mvp = camera.proj * camera.view; //* push.model;
    for (uint i = 0; i < 4; i++)
    {
        gl_MeshVerticesEXT[v_offset + i].gl_Position = mvp * generated[i].position;
    }

    m_out[v_offset + 0].color = vec3(0, 1, 1);
    m_out[v_offset + 1].color = vec3(1, 1, 1);
    m_out[v_offset + 2].color = vec3(1, 1, 1);
    m_out[v_offset + 3].color = vec3(1, 0, 1);

    gl_PrimitiveTriangleIndicesEXT[i_offset + 0] = uvec3(2, 1, 0) + v_offset;
    gl_PrimitiveTriangleIndicesEXT[i_offset + 1] = uvec3(3, 2, 0) + v_offset;
}