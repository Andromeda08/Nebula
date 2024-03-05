#version 460
#extension GL_EXT_mesh_shader                            : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int8  : require
#extension GL_EXT_buffer_reference2                      : require
#extension GL_EXT_scalar_block_layout                    : enable
#extension GL_GOOGLE_include_directive                   : enable
#extension GL_KHR_shader_subgroup_basic                  : require
#extension GL_KHR_shader_subgroup_ballot                 : require
#extension GL_KHR_shader_subgroup_vote                   : require
#extension GL_EXT_debug_printf                           : enable

#define WORKGROUP_SIZE 32

layout (local_size_x = WORKGROUP_SIZE) in;
layout (triangles, max_vertices = 96, max_primitives = 84) out;

struct Task {
    uint    baseID;
    uint8_t deltaID[WORKGROUP_SIZE - 1];
};
taskPayloadSharedEXT Task IN;

layout (push_constant) uniform HairConstants {
    mat4     model;
    ivec4    buffer_lengths;        // [ VTX, SDESC, -, - ]
    uint64_t vertex_buffer;
    uint64_t strand_desc_buffer;
} hair_constants;

layout (binding = 0) uniform CameraData {
    mat4 view;
    mat4 proj;
    mat4 view_inverse;
    mat4 proj_inverse;
    vec4 eye;
} camera;

layout (buffer_reference, scalar) buffer Vertices {
    vec4 positions[];
};
layout (buffer_reference, scalar) buffer StrandDescriptions {
    ivec4 strand_descs[];
};

struct MeshOut {
    vec3 color;
};
layout (location = 0) out MeshOut m_out[];

uint laneID = gl_LocalInvocationID.x;

// Functions
ivec4 get_strand_desc(uint id)
{
    StrandDescriptions descs = StrandDescriptions(hair_constants.strand_desc_buffer);
    return descs.strand_descs[id];
}

vec4[4] generate_quad(uint offset)
{
    Vertices v = Vertices(hair_constants.vertex_buffer);
    vec4 result[4];

    result[0] = v.positions[offset + 0];
    result[1] = v.positions[offset + 0];
    result[2] = v.positions[offset + 1];
    result[3] = v.positions[offset + 1];

    result[1] += vec4(1, 0, 0, 0);
    result[2] += vec4(1, 0, 0, 0);

    return result;
}

void main()
{
    uint deltaID = 0;
    for (uint i = 0; i < 32; i++) {
        if (gl_WorkGroupID.x < uint(IN.deltaID[i])) break;
        deltaID = uint(IN.deltaID[i]);
    }

    uint strand_id = IN.baseID + deltaID;
    ivec4 strand_desc = get_strand_desc(strand_id);

    uint strandlet_count = strand_desc.z;
    uint vertex_count    = strand_desc.y;
    uint strandlet = deltaID - gl_WorkGroupID.x;

    if (vertex_count - (strandlet * 32) + laneID < 0) {
        return;
    }

    uvec4 vote  = subgroupBallot(true);
    uint  count = subgroupBallotBitCount(vote);

    uint i_offset	= laneID * 2;
    uint v_offset	= laneID * 4;
    uint quads 	    = count - 1;
    uint vtx_count  = quads * 4;
    uint tri_count  = quads * 2;

    SetMeshOutputsEXT(vtx_count, tri_count);

    const mat4 mvp = camera.proj * camera.view * hair_constants.model;
    const vec4 generated[4] = generate_quad(strand_id + (strandlet * 32) + gl_LocalInvocationID.x);
    for (uint i = 0; i < 4; i++)
    {
        gl_MeshVerticesEXT[v_offset + i].gl_Position = mvp * generated[i];
    }

    m_out[v_offset + 0].color = vec3(0, 1, 1);
    m_out[v_offset + 1].color = vec3(1, 1, 1);
    m_out[v_offset + 2].color = vec3(1, 1, 1);
    m_out[v_offset + 3].color = vec3(1, 0, 1);

    gl_PrimitiveTriangleIndicesEXT[i_offset + 0] = uvec3(2, 1, 0) + v_offset;
    gl_PrimitiveTriangleIndicesEXT[i_offset + 1] = uvec3(3, 2, 0) + v_offset;
}
