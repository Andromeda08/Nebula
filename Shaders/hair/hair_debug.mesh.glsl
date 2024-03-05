#version 460
#extension GL_EXT_mesh_shader : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : enable
#extension GL_KHR_shader_subgroup_basic : require
#extension GL_KHR_shader_subgroup_ballot : require
#extension GL_KHR_shader_subgroup_vote : require
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

// Description: [ ID, Vertex Count, Strandlet Count, Vertex_Offset ]
ivec4 getStrandDescription(uint id) {
    StrandDescriptions sds = StrandDescriptions(hair_constants.sdesc_address);
    return sds.descriptions[id].desc;
}

Vertex[4] build_quad(uint offset) {
    Vertices v = Vertices(hair_constants.vertex_address);
    Vertex result[4];

    result[0] = v.vertices[offset + 0];

    result[1] = v.vertices[offset + 0];
    result[1].position += vec4(1, 0, 0, 0);

    result[2] = v.vertices[offset + 1];
    result[2].position += vec4(1, 0, 0, 0);

    result[3] = v.vertices[offset + 1];

    return result;
}

vec4 getVertexColor(uint id) {
    if (id == 0) {
        return vec4(0, 1, 1, 1);
    }
    if (id == 1 || id == 2) {
        return vec4(1, 1, 1, 1);
    }
    if (id == 3) {
        return vec4(1, 0, 1, 1);
    }
    return vec4(0, 0, 0, 1);
}

uint currentStrandletVertexCount(uint i, uint vtx_count) {
    if (vtx_count < 32) {
        if (i == 0) return vtx_count;
        else        return 0;
    }
    return clamp(vtx_count - (WORKGROUP_SIZE * (i - 1)), 0, WORKGROUP_SIZE);
}

void main()
{
    uint deltaID = 0;
    for (uint i = 0; i < WORKGROUP_SIZE; i++) {
        if (workGroupID < uint(IN.deltaID[i])) break;
        deltaID = uint(IN.deltaID[i]);
    } 

    uint  strand_id    = IN.baseID + deltaID;
    ivec4 strand_desc  = getStrandDescription(strand_id);
    uint  n_strandlets = strand_desc.z;
    uint  n_vertices   = strand_desc.y;
    uint  n_prev_vtxs  = strand_desc.w;
    uint  strandlet_id = deltaID - workGroupID;
    uint  vtx_in_slet  = currentStrandletVertexCount(strandlet_id, n_vertices);

    if (laneID > vtx_in_slet) {
        return;
    }
    
    uint n_quads = vtx_in_slet - 1;
    uint n_vtx   = n_quads * 4;
    uint n_tri   = n_quads * 2;

    SetMeshOutputsEXT(n_vtx, n_tri);

    uint vertex_buffer_offset = n_prev_vtxs + clamp((int(strandlet_id) - 1) * WORKGROUP_SIZE, 0, WORKGROUP_SIZE) + laneID;

    if (laneID > 20 && laneID <= vtx_in_slet) { 
        debugPrintfEXT(
            "Hello from Mesh WG #%d Lane #%d\n\tWorking on SID #%d, SletID %d (of %d)\n\tCurrent Slet Vtx Count = %d - (%d * (%d - 1)) = %d\n\tQuad count: %d -> V: %d, T %d\n\tVBOffset = %d + clamp((%d - 1) * %d, 0, %d) + %d = %d\n",
            workGroupID, laneID,
            strand_id, strandlet_id, n_strandlets,
            n_vertices, WORKGROUP_SIZE, strandlet_id, vtx_in_slet,
            n_quads, n_vtx, n_tri,
            n_prev_vtxs, strandlet_id, WORKGROUP_SIZE, WORKGROUP_SIZE, laneID, vertex_buffer_offset);
    }

    const Vertex quad[4] = build_quad(vertex_buffer_offset);

    const uint vtx_out_offset = laneID * 4;
    const uint tri_out_offset = laneID * 2;

    const mat4 MVP = camera.proj * camera.view * hair_constants.model;
    for (uint i = 0; i < 4; i++) {
        gl_MeshVerticesEXT[vtx_out_offset + i].gl_Position = MVP * quad[i].position;
        m_out[vtx_out_offset + i].color = getVertexColor(i);
    }

    gl_PrimitiveTriangleIndicesEXT[tri_out_offset + 0] = uvec3(2, 1, 0) + vtx_out_offset;
    gl_PrimitiveTriangleIndicesEXT[tri_out_offset + 1] = uvec3(3, 2, 0) + vtx_out_offset;
}