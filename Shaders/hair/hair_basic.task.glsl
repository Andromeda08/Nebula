#version 460
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_mesh_shader : require

#define WORKGROUP_SIZE 32

layout (local_size_x = WORKGROUP_SIZE) in;

layout (push_constant) uniform HairConstants {
    mat4     model;
    ivec4    buffer_lengths;        // [ VTX, SDESC, -, - ]
    uint64_t vertex_address;
    uint64_t strand_desc_address;
} push;

void main()
{
  if (gl_WorkGroupID.x == 0 && gl_LocalInvocationID.x == 0) {
    debugPrintfEXT("Hi from Task WG #0 & Lane #0\n");
  }

  EmitMeshTasksEXT(30, 1, 1);
}