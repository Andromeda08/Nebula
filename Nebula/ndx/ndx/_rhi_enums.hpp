#pragma once

#include <cstdint>

namespace Nebula::ndx
{
    enum class Format
    {
        eUnknown,
        eR8Srgb,
        eR8Unorm,
        eR8G8Srgb,
        eR8G8Unorm,
        eR8G8B8Srgb,
        eR8G8B8Unorm,
        eR8G8B8A8Srgb,
        eR8G8B8A8Unorm,
        eB8G8R8A8Unorm,
        eB8G8R8A8Srgb,
        eR16Sfloat,
        eR16G16Sfloat,
        eR16G16B16Sfloat,
        eR16G16B16A16Sfloat,
        eR32Sfloat,
        eR32G32Sfloat,
        eR32G32B32Sfloat,
        eR32G32B32A32Sfloat,
        eD16Unorm,
        eX8D24UnormPack32,
        eD32Sfloat,
        eS8Uint,
        eD16UnormS8Uint,
        eD24UnormS8Uint,
        eD32SfloatS8Uint
    };

    enum class ShaderStage
    {
        eVertex,
        eTesselationControl,
        eTesselationEval,
        eGeometry,
        eFragment,
        eCompute,
        eRayGen,
        eClosestHit,
        eMiss,
        eInvokable,
        eMesh,
        eTask
    };

    enum class VertexInputRate
    {
        eInstance,
        eVertex,
    };

    // Rasterization State Enums --------------------------------------
    enum class CullMode
    {
        eNone,
        eBack,
        eFront,
    };

    enum class FillMode
    {
        eSolid,
        eWireframe,
    };

    enum class SampleCount : uint32_t
    {
        e1  = 1,
        e2  = 2,
        e4  = 4,
        e8  = 8,
        e16 = 16,
        e32 = 32,
        e64 = 64
    };

    // Pipeline Parameters  -------------------------------------------
    enum class PipelineType
    {
        eCompute,
        eGraphics,
        eRayTracing,
        eUnknown
    };
}