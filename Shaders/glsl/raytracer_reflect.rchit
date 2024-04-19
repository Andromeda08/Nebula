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
layout(binding = 3, set = 0) buffer  ObjDesc { ObjectDescription object_description; };

layout(push_constant) uniform RTPushConstant { LightData light; } constants;

layout(location = 0)  rayPayloadInEXT RTPayload payload;
layout(location = 1)  rayPayloadEXT   bool      is_shadowed;

layout(location = 0) callableDataEXT RayLight ray_light;

// Fetch primitive and calculate position and normal
void fetch_vertex_data(int primitiveID, out vec3 position, out vec3 normal) {
    Vertices vertices = Vertices(object_description.vertex_address);
    Indices  indices  = Indices(object_description.index_address);
    ivec3    index    = indices.i[gl_PrimitiveID];

    Vertex v0 = vertices.v[index.x];
    Vertex v1 = vertices.v[index.y];
    Vertex v2 = vertices.v[index.z];

    const vec3 barycentrics = vec3(1.0 - attributes.x - attributes.y, attributes.x, attributes.y);

    position = v0.position * barycentrics.x + v1.position * barycentrics.y + v2.position * barycentrics.z;
    normal = v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z;
}

void main() {
    vec3 position;
    vec3 normal;
    fetch_vertex_data(gl_PrimitiveID, position, normal);

    const vec3 world_pos    = vec3(gl_ObjectToWorldEXT * vec4(position, 1.0));
    const vec3 world_normal = normalize(vec3(normal * gl_WorldToObjectEXT));

    vec3 N = world_normal;

    // Light
    ray_light.in_hit_position = world_pos;
    executeCallableEXT(constants.light.light_type, 0);

    vec3  L               = ray_light.out_light_dir;
    vec3  light_intensity = ray_light.out_light_intensity;
    float light_distance  = ray_light.out_light_distance;

//    if (constants.light.light_position.w == 0.0) {
//        // Point Light
//        vec3 light_direction = constants.light.light_position.xyz - world_pos;
//        light_distance  = length(light_direction);
//        // light_intensity = constants.light.light_intensity.rgb / (light_distance * light_distance);
//        L               = normalize(light_direction);
//    }
//    else {
//        // Directional Light
//        L = normalize(constants.light.light_position.xyz);
//    }

    vec2 id       = vec2(gl_InstanceID, gl_InstanceID);
    // vec3 color    = vec3(rand(id) * sin(id.x) + 1.0, rand(id) * cos(id.y) + 1.0, rand(id));
    vec3 color    = vec3(0.75);
    vec3 diffuse  = compute_diffuse(color, L, world_normal);
    vec3 specular = vec3(0.0);

    float attenuation = 1.0;

    if (dot(N, L) > 0)
    {
        vec4 origin    = vec4(gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT, 1.0);
        vec4 direction = vec4(L, 0.0);
        uint ray_flags = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
        is_shadowed    = true;

        traceRayEXT(tlas, ray_flags, 0xff, 0, 0, 0,
                    origin.xyz, 0.001, direction.xyz, light_distance,
                    0);
    }

    if (is_shadowed) {
        attenuation = 0.3;
    }
    else {
        specular = compute_specular(color, gl_WorldRayDirectionEXT, L, world_normal);
    }

    payload.ray_origin    = world_pos;
    payload.ray_direction = reflect(gl_WorldRayDirectionEXT, world_normal);
    payload.attenuation  *= vec3(attenuation);
    payload.done          = 0;
    payload.hit_value     = vec3(diffuse + specular) * light_intensity;
}