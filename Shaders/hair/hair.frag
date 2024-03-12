#version 460

#extension GL_GOOGLE_include_directive : enable
#include "hair_common.glsl"

layout(early_fragment_tests) in;

layout (location = 0) in Mesh IN;

layout (binding = 0) uniform CameraData {
    mat4 view;
    mat4 proj;
    mat4 view_inverse;
    mat4 proj_inverse;
    vec4 eye;
} camera;

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

vec3 kajiya_kay(vec3 diffuse, vec3 specular, float p, vec3 T, vec3 L, vec3 E)
{
    float cosTL    = dot(T, L);
    float cosTL_sq = cosTL * cosTL;

    float cosTE    = dot(T, E);
    float cosTE_sq = cosTE * cosTE;

    float sinTL    = sqrt(1.0f - cosTL_sq);
    float sinTE    = sqrt(1.0f - cosTE_sq);

    vec3 d = diffuse * sinTL;
    vec3 s = specular * pow(max(cosTL * cosTE + sinTL * sinTE, 0), p);

    return d + s;
}

void main()
{
    vec4 light = vec4(-75, 125, 50, 0);

    vec3 hairD = vec3(83, 61, 53) / 255;
    vec3 hairS = vec3(106, 78, 56) / 255;

    vec3 T = normalize(IN.world_tangent.xyz);
    vec3 L = normalize(light - IN.world_position).xyz;

    vec3 color = kajiya_kay(hairD, hairS, 2.0, T, L, camera.eye.xyz);
    out_color = vec4(color, 1.0);
}