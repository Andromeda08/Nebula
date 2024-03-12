#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_QUALITY__PRESET 39
#define FXAA_GREEN_AS_LUMA 1

#include "inc/fxaa.hlsl"

static const float fxaaSubpix = 0.75;
static const float fxaaEdgeThreshold = 0.166;
static const float fxaaEdgeThresholdMin = 0.0833;

struct PushConstant
{
    float2 resolution_rcp;
};

#if defined(_DXC)
[[vk::push_constant]] PushConstant pc;
#else
[[vk::push_constant]] ConstantBuffer<PushConstant> pc;
#endif

[[vk::combinedImageSampler]]
[[vk::binding(0)]]
Texture2D<float4> inputImage;

[[vk::combinedImageSampler]]
[[vk::binding(0)]]
SamplerState inputSampler;

float4 main([[vk::location(0)]] float2 inUV : TEXCOORD0) : SV_TARGET
{
    const float2 uv = float2(inUV.x, -inUV.y);
    FxaaTex tex = { inputSampler, inputImage };
    return FxaaPixelShader(uv, 0, tex, tex, tex, pc.resolution_rcp, 0, 0, 0, fxaaSubpix, fxaaEdgeThreshold, fxaaEdgeThresholdMin, 0, 0, 0, 0);
}