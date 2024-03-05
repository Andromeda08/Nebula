#version 460
#extension GL_EXT_mesh_shader : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : enable
#extension GL_KHR_shader_subgroup_arithmetic : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

#extension GL_EXT_debug_printf : enable

#extension GL_GOOGLE_include_directive : enable
#include "hair_common.glsl"

layout (local_size_x = WORKGROUP_SIZE) in;

layout (push_constant) uniform HairConstants {
    mat4     model;
    ivec4    buffer_lengths;  // [ VTX, SDESC, -, - ]
    uint64_t vertex_address; 
    uint64_t sdesc_address;
} hair_constants;

layout (buffer_reference, scalar) buffer StrandDescriptions {
    StrandDesc descriptions[];
};

// Input --------------------------------
uint baseID = gl_WorkGroupID.x * WORKGROUP_SIZE;
uint laneID = gl_LocalInvocationID.x;

// Output -------------------------------
taskPayloadSharedEXT Task OUT;

// Functions ----------------------------
int getStrandCount() { return hair_constants.buffer_lengths.y; }

// Description: [ ID, Vertex Count, Strandlet Count, Vertex_Offset ]
ivec4 getStrandDescription(uint id) {
    StrandDescriptions sds = StrandDescriptions(hair_constants.sdesc_address);
    return sds.descriptions[id].desc;
}

void main()
{
    uint out_strandlet_count = 0;

    uint strand_local  = laneID;
    uint strand_global = baseID + strand_local;
    
    if (strand_global >= getStrandCount()) {
        return;
    }

    ivec4 strand_desc  = getStrandDescription(strand_global);
    int   n_strandlets = strand_desc.z;

    out_strandlet_count += n_strandlets;

    uint strand_work_group_offset = subgroupExclusiveAdd(n_strandlets);
    if (laneID != 0) {
        OUT.deltaID[laneID - 1] = uint8_t(strand_work_group_offset);
    }
    OUT.baseID = baseID;

#ifdef DEBUG
    if (laneID > 30) {
        debugPrintfEXT(
            "Hello from Task WG #%d Lane #%d\n\tout_strandlet_count = %d\n\tWorking on strand #%d, will spawn %d Mesh Shader work groups\n",
            baseID, laneID, out_strandlet_count, strand_global, n_strandlets);
    }
#endif

    EmitMeshTasksEXT(out_strandlet_count, 1, 1);
}