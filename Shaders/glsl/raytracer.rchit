#version 460

#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

#include "rt_common.glsl"

struct ObjectDescription {
    uint64_t vertex_address;
    uint64_t index_address;
};

hitAttributeEXT vec2 attributes;

layout(buffer_reference, scalar) buffer Vertices { Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices  { ivec3  i[]; };

layout(binding = 0, set = 0) uniform accelerationStructureEXT tlas;
layout(binding = 3, set = 0) buffer ObjDesc { ObjectDescription obj_desc; };

layout(location = 0) rayPayloadInEXT RTPayloadBasic payload;

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    Vertices vertices = Vertices(obj_desc.vertex_address);
    Indices  indices  = Indices(obj_desc.index_address);

    ivec3 idx = indices.i[gl_PrimitiveID];

    Vertex v0 = vertices.v[idx.x];
    Vertex v1 = vertices.v[idx.y];
    Vertex v2 = vertices.v[idx.z];

    const vec3 barycentrics = vec3(1.0 - attributes.x - attributes.y, attributes.x, attributes.y);

    const vec3 pos = v0.position * barycentrics.x + v1.position * barycentrics.y + v2.position * barycentrics.z;
    const vec3 world_pos = vec3(gl_ObjectToWorldEXT * vec4(pos, 1.0));

    const vec3 normal = v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z;
    const vec3 world_normal = normalize(vec3(normal * gl_WorldToObjectEXT));

    vec3 N = world_normal;
    vec3 l_dir = vec3(-12, 10, 5) - world_pos;

    vec3 L = normalize(l_dir);
    float light_distance = length(l_dir);

    vec2 id = vec2(gl_InstanceID, gl_InstanceID);
    vec3 color = vec3(rand(id) * sin(id.x) + 1.0, rand(id) * cos(id.y) + 1.0, rand(id));
    vec3 diffuse  = compute_diffuse(color, L, world_normal);
    vec3 specular = compute_specular(color, gl_WorldRayDirectionEXT, L, world_normal);

    payload.hit_value = vec3(diffuse + specular);
}