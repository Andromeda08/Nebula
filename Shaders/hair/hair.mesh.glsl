#version 460
#extension GL_EXT_mesh_shader : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

#extension GL_EXT_debug_printf : enable

#extension GL_GOOGLE_include_directive : enable
#include "hair_common.glsl"

layout (local_size_x = WORKGROUP_SIZE) in;
layout (triangles, max_vertices = 128, max_primitives = 64) out;

layout (push_constant) uniform HairConstants {
    mat4     model;
    ivec4    buffer_lengths;  // [ VTX, SDESC, -, - ]
    uint64_t vertex_address; 
    uint64_t sdesc_address;
} hair_constants;

layout (buffer_reference, scalar) buffer Vertices {
    Vertex vertices[];
};

layout (buffer_reference, scalar) buffer StrandDescriptions {
    StrandDesc descriptions[];
};

layout (binding = 0) uniform CameraData {
    mat4 view;
    mat4 proj;
    mat4 view_inverse;
    mat4 proj_inverse;
    vec4 eye;
} camera;

// Input --------------------------------
taskPayloadSharedEXT Task IN;

uint workGroupID = gl_WorkGroupID.x;
uint laneID      = gl_LocalInvocationID.x;

// Output -------------------------------
layout (location = 0) out Mesh m_out[];

// Functions ----------------------------
int getStrandCount() { return hair_constants.buffer_lengths.y; }

StrandDesc getStrandDescription(uint id) {
    StrandDescriptions sds = StrandDescriptions(hair_constants.sdesc_address);
    return sds.descriptions[id];
}

Vertex[4] build_quad(uint offset) {
    Vertices v = Vertices(hair_constants.vertex_address);
    Vertex result[4];

    result[0] = v.vertices[offset + 0];
    result[1] = v.vertices[offset + 0];
    result[2] = v.vertices[offset + 1];
    result[3] = v.vertices[offset + 1];

    float t = 0.15;
    result[1].position += vec4(t, 0, 0, 0);
    result[2].position += vec4(t, 0, 0, 0);

    return result;
}

vec4 getVertexColor(uint id, uint sid) {
    if (id == 0) {
        return vec4(0, 1, 1, 1);
    }
    if (id == 1 || id == 2) {
        if (laneID == 0) return vec4(1, 1, 1, 1);
        return get_meshlet_color(sid);
    }
    if (id == 3) {
        return vec4(1, 0, 1, 1);
    }
    return vec4(0, 0, 0, 1);
}

uint getStrandletVertexCount(uint strandletID, uint totalVertexCount) {
    return min(totalVertexCount - (WORKGROUP_SIZE * strandletID), WORKGROUP_SIZE);
}

uint getGlobalVertexBufferOffset(uint baseOffset, uint strandletID) {
    return baseOffset + strandletID * (WORKGROUP_SIZE) + laneID;
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
    uint n_vtx   = n_quads * 4;
    uint n_tri   = n_quads * 2;

    // Do no work if current lane exceeds quad count
    if (laneID > n_quads) return;

    SetMeshOutputsEXT(n_vtx, n_tri);

    // Calculate global offset into the vertex buffer
    uint vertex_buffer_offset = base_vertex_offset + (strandletID * WORKGROUP_SIZE) + laneID - strandletID;

    const Vertex quad[4] = build_quad(vertex_buffer_offset);

    const vec4 world_tangent = hair_constants.model * vec4((quad[3].position - quad[0].position).xyz, 0);

    const uint vtx_out_offset = laneID * 4;
    const uint tri_out_offset = laneID * 2;

    const mat4 VP = camera.proj * camera.view;
    for (uint i = 0; i < 4; i++) {
        vec4 world_position = hair_constants.model * quad[i].position;

        gl_MeshVerticesEXT[vtx_out_offset + i].gl_Position = VP * world_position;

        m_out[vtx_out_offset + i].color          = getVertexColor(i, current_strandID);
        m_out[vtx_out_offset + i].world_position = world_position;
        m_out[vtx_out_offset + i].world_tangent  = world_tangent;
        m_out[vtx_out_offset + i].uv             = vec2((i << 1) & 2, i & 2);
    }

    gl_PrimitiveTriangleIndicesEXT[tri_out_offset + 0] = uvec3(2, 1, 0) + vtx_out_offset;
    gl_PrimitiveTriangleIndicesEXT[tri_out_offset + 1] = uvec3(3, 2, 0) + vtx_out_offset;
}