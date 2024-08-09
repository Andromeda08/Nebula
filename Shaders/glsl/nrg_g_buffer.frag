#version 460

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inWorldNormal;
layout (location = 2) in vec2 inUv;
layout (location = 3) in vec3 inColor;
layout (location = 4) in vec3 inViewDir;
layout (location = 5) in vec4 inCurrentPosition;
layout (location = 6) in vec4 inPreviousPosition;

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outAlbedo;
layout (location = 3) out vec4 outMotionVector;

void main()
{
    vec3 N = normalize(inWorldNormal);

    outPosition = vec4(inWorldPos, 1);
    outNormal = vec4(N, 1);
    outAlbedo = vec4(inColor, 1);

    vec3 a = (inCurrentPosition / inCurrentPosition.w).xyz;
    vec3 b = (inPreviousPosition / inPreviousPosition.w).xyz;
    vec2 mvec = (a - b).xy * 0.5;

    outMotionVector = vec4(mvec, 0.0, 1.0);
}

