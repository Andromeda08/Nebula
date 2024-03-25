#pragma once

#include <string>
#include <stdexcept>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "_rhi_enums.hpp"

namespace Nebula::ndx
{
    inline DXGI_FORMAT convert(const Format format)
    {
        switch (format)
        {
            using enum Format;
            case eUnknown:              return DXGI_FORMAT_UNKNOWN;
            case eR8Unorm:              return DXGI_FORMAT_R8_UNORM;
            case eR8G8Unorm:            return DXGI_FORMAT_R8G8_UNORM;
            case eR8G8B8A8Srgb:         return DXGI_FORMAT_R8G8B8A8_SINT;
            case eR8G8B8A8Unorm:        return DXGI_FORMAT_R8G8B8A8_UNORM;
            case eB8G8R8A8Srgb:         return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            case eB8G8R8A8Unorm:        return DXGI_FORMAT_B8G8R8A8_UNORM;
            case eR16Sfloat:            return DXGI_FORMAT_R16_FLOAT;
            case eR16G16Sfloat:         return DXGI_FORMAT_R16G16_FLOAT;
            case eR16G16B16A16Sfloat:   return DXGI_FORMAT_R16G16B16A16_FLOAT;
            case eR32Sfloat:            return DXGI_FORMAT_R32_FLOAT;
            case eR32G32Sfloat:         return DXGI_FORMAT_R32G32_FLOAT;
            case eR32G32B32Sfloat:      return DXGI_FORMAT_R32G32B32_FLOAT;
            case eR32G32B32A32Sfloat:   return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case eD16Unorm:             return DXGI_FORMAT_D16_UNORM;
            case eX8D24UnormPack32:     return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
            case eD32Sfloat:            return DXGI_FORMAT_D32_FLOAT;
            case eD24UnormS8Uint:       return DXGI_FORMAT_D24_UNORM_S8_UINT;
            case eD32SfloatS8Uint:      return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

            case eR8Srgb:
            case eR8G8Srgb:
            case eR8G8B8Srgb:
            case eR8G8B8Unorm:
            case eR16G16B16Sfloat:
            case eS8Uint:
            case eD16UnormS8Uint:
            default:
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    inline D3D12_INPUT_CLASSIFICATION convert(const VertexInputRate vertex_input_rate)
    {
        switch (vertex_input_rate)
        {
            case VertexInputRate::eInstance:
                return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
            case VertexInputRate::eVertex:
                return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        }
        throw std::invalid_argument("Invalid parameter");
    }

    inline D3D12_CULL_MODE convert(const CullMode cull_mode)
    {
        switch (cull_mode)
        {
            using enum CullMode;
            case eNone:  return D3D12_CULL_MODE_NONE;
            case eBack:  return D3D12_CULL_MODE_BACK;
            case eFront: return D3D12_CULL_MODE_FRONT;
        }
        throw std::invalid_argument("Invalid parameter");
    }

    inline D3D12_FILL_MODE convert(const FillMode fill_mode)
    {
        switch (fill_mode)
        {
            using enum FillMode;
            case FillMode::eSolid:     return D3D12_FILL_MODE_SOLID;
            case FillMode::eWireframe: return D3D12_FILL_MODE_WIREFRAME;
        }
        throw std::invalid_argument("Invalid parameter");
    }
}