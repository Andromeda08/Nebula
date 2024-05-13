#version 460
#extension GL_EXT_mesh_shader : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

#extension GL_EXT_debug_printf : enable

#extension GL_GOOGLE_include_directive : enable
#include "hair_common.glsl"

layout (local_size_x = WORKGROUP_SIZE) in;
layout (triangles, max_vertices = 64, max_primitives = 64) out;

layout (push_constant) uniform HairConstants {
    mat4     model;
    ivec4    buffer_lengths;
    vec4     hair_color;
    vec4     hair_specular;
    uint64_t vertex_address; 
    uint64_t sdesc_address;
} hair_constants;

layout (buffer_reference, scalar) buffer Vertices { Vertex vertices[]; };

layout (buffer_reference, scalar) buffer StrandDescriptions { StrandDesc descriptions[]; };

layout (binding = 0) uniform CameraData {
    mat4  view;
    mat4  proj;
    mat4  view_inverse;
    mat4  proj_inverse;
    vec4  eye;
    float near_plane;
    float far_plane;
} camera;

// Input --------------------------------
taskPayloadSharedEXT Task IN;

uint workGroupID = gl_WorkGroupID.x;
uint laneID      = gl_LocalInvocationID.x;

// Output -------------------------------
layout (location = 0) out MeshData m_out[];

// Functions ----------------------------
StrandDesc getStrandDescription(uint id) {
    StrandDescriptions sds = StrandDescriptions(hair_constants.sdesc_address);
    return sds.descriptions[id];
}

uint getStrandletVertexCount(uint strandletID, uint totalVertexCount) {
    return min(totalVertexCount - (WORKGROUP_SIZE * strandletID), WORKGROUP_SIZE);
}

void main()
{
    uint deltaID = 0;
    uint k = 0;
    for (uint i = 0; i < WORKGROUP_SIZE; i++) {
        if (workGroupID < uint(IN.deltaID[i])) break;
        deltaID = uint(IN.deltaID[i]);
        k = i;
    }

    // Current [Strand] information
    uint       current_strandID    = IN.baseID + k;
    StrandDesc strand_description  = getStrandDescription(current_strandID);
    uint       strand_vertex_count = strand_description.vertex_count;
    uint       base_vertex_offset  = strand_description.vertex_offset;

    // Current [Strandlet] information
    uint strandletID        = workGroupID - deltaID;
    uint strandlet_vertices = getStrandletVertexCount(strandletID, strand_vertex_count);
    
    // Calculate output parameters
    uint n_quads = strandlet_vertices - 1;
    uint n_tri   = n_quads * 2;
    uint n_vtx   = n_quads * 4;

    // Do no work if current lane exceeds quad count
    if (laneID > n_quads) return;

    SetMeshOutputsEXT(n_vtx, n_tri);

    // Calculate global offset into the vertex buffer
    uint vertex_buffer_offset = base_vertex_offset + (strandletID * WORKGROUP_SIZE) + laneID - strandletID;
    Vertices vertex_buffer = Vertices(hair_constants.vertex_address);

    vec4 strand_vertex = vertex_buffer.vertices[vertex_buffer_offset].position;
    vec4 offset_vertex = strand_vertex + vec4(0.15, 0.0, 0.0, 0.0);
    vec4 next_vertex   = vertex_buffer.vertices[vertex_buffer_offset + 1].position;

    vec4 tangent = vec4(strand_vertex.xyz - next_vertex.xyz, 0.0);

    // [Strand (0) | Offset (1) | Offset (2) | Strand (3)] ordered output
    // 0 ---- 1
    // |    / |
    // | /    |
    // 3 ---- 2
    vec4 A = (laneID % 2 == 0) ? strand_vertex : offset_vertex;
    vec4 B = (laneID % 2 == 0) ? offset_vertex : strand_vertex;

    const mat4 M  = hair_constants.model;
    const mat4 VP = camera.proj * camera.view;

    vec4 world_pos_A   = M * A;
    vec4 world_pos_B   = M * B;
    vec4 world_tangent = normalize(vec4((M * tangent).xyz, 0.0));

    const uint out_offset = laneID * 2;

    gl_MeshVerticesEXT[out_offset + 0].gl_Position = VP * world_pos_A;
    m_out[out_offset + 0].world_position = world_pos_A;
    m_out[out_offset + 0].world_tangent  = world_tangent;

    gl_MeshVerticesEXT[out_offset + 1].gl_Position = VP * world_pos_B;
    m_out[out_offset + 1].world_position = world_pos_B;
    m_out[out_offset + 1].world_tangent  = world_tangent;

    const uint tri_offset = laneID * 2;
    gl_PrimitiveTriangleIndicesEXT[tri_offset + 0] = uvec3(2, 1, 0) + out_offset;
    gl_PrimitiveTriangleIndicesEXT[tri_offset + 1] = uvec3(3, 2, 0) + out_offset;
}
