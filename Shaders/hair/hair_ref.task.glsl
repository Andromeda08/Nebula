#version 460

#extension GL_EXT_mesh_shader                            : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int8  : require
#extension GL_KHR_shader_subgroup_arithmetic             : enable
#extension GL_EXT_buffer_reference2                      : require
#extension GL_EXT_scalar_block_layout                    : enable
#extension GL_EXT_debug_printf                           : enable

#define WORKGROUP_SIZE 32

layout (local_size_x = WORKGROUP_SIZE) in;

layout (push_constant) uniform HairConstants {
    mat4     model;
    ivec4    buffer_lengths;        // [ VTX, SDESC, -, - ]
    uint64_t vertex_buffer;
    uint64_t strand_desc_buffer;
} pushc;

// [ strand_id, vertex_count, strandlet_count, - ]
layout (buffer_reference, scalar) buffer StrandDescriptions {
    ivec4 strand_descs[];
};

// Input
uint baseID = gl_WorkGroupID.x * WORKGROUP_SIZE;
uint laneID = gl_LocalInvocationID.x;

// Output
struct Task {
    uint    baseID;
    uint8_t deltaID[WORKGROUP_SIZE - 1];
};
taskPayloadSharedEXT Task OUT;

// Functions
ivec4 get_strand_desc(uint id)
{
    StrandDescriptions descs = StrandDescriptions(pushc.strand_desc_buffer);
    return descs.strand_descs[id];
}

void main()
{
    if (gl_WorkGroupID.x == 0 && gl_LocalInvocationID.x == 0) {
        debugPrintfEXT("Hi from Task WG #0 & Lane #0");
    }

    uint  strand_local  = laneID;
    uint  strand_global = baseID + strand_local;
    if (strand_global >= pushc.buffer_lengths.y) {
        return;
    }

    ivec4 strand_desc   = get_strand_desc(strand_global);
    int   strandlet_cnt = strand_desc.z;

    uint  strand_offset = subgroupExclusiveAdd(strandlet_cnt);

    if (laneID != 0) {
        OUT.deltaID[laneID - 1] = uint8_t(strand_offset);
    }
    OUT.baseID = baseID;

    EmitMeshTasksEXT(pushc.buffer_lengths.y, 1, 1);
}