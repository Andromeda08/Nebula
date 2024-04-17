#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable

#include "rt_common.glsl"

layout(location = 0) rayPayloadInEXT RTPayloadBasic payload;

void main()
{
    payload.hit_value = vec3(0.05);
}