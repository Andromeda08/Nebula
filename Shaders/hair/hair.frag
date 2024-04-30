#version 460

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#include "hair_common.glsl"

layout(early_fragment_tests) in;

layout (location = 0) in Mesh IN;

layout (binding = 0) uniform CameraData {
    mat4  view;
    mat4  proj;
    mat4  view_inverse;
    mat4  proj_inverse;
    vec4  eye;
    float near_plane;
    float far_plane;
} camera;

layout (push_constant) uniform HairConstants {
    mat4     model;
    ivec4    buffer_lengths;
    vec4     hair_diffuse;
    vec4     hair_specular;
    uint64_t vertex_address;
    uint64_t sdesc_address;
} hair_constants;

layout (location = 0) out vec4 out_color;

vec3 shift_tangent(vec3 T, vec3 N, float s)
{
    return normalize(T + s * N);
}

float strand_specular(vec3 T, vec3 V, vec3 L, float exponent)
{
    vec3  H     = normalize(L + V);
    float dotTH = dot(T, H);
    float sinTH = sqrt(1.0 - dotTH * dotTH);
    float dir_attenuation = smoothstep(-1.0, 0.0, dotTH);
    return dir_attenuation * pow(sinTH, exponent);
}

float strand_diffuse(vec3 N, vec3 L)
{
    return clamp(mix(0.25, 1.0, max(dot(N, L), 0.0)), 0.0, 1.0);
}

vec3 kajiya_kay(vec3 diffuse, vec3 specular, float p, vec3 T, vec3 L, vec3 V) {
    float cosTL    = dot(T, L);
    float sinTL    = sqrt(1.0f - cosTL * cosTL);

    vec3 H      = normalize(L + V);
    float cosTH = dot(T, H);
    float sinTH = sqrt(1.0 - cosTH * cosTH);

    vec3 d = diffuse * sinTL;
    vec3 s = specular * pow(max(sinTH, 0.0), p);

    return d + s;
}

void main()
{
    vec4 light = vec4(-75, 125, 50, 0);

    vec3 T = normalize(IN.world_tangent.xyz);
    vec3 L = normalize(light - IN.world_position).xyz;
    vec3 V = normalize(camera.eye.xyz - IN.world_position.xyz);

    vec3 color = kajiya_kay(hair_constants.hair_diffuse.xyz, hair_constants.hair_specular.xyz, 16.0, T, L, V);
    out_color = vec4(color, 0.75);
}