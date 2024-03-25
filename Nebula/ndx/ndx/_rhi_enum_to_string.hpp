#pragma once

#include <string>
#include "_rhi_enums.hpp"

#ifndef enum2str_case
#define enum2str_case(Enum) case Enum: return #Enum;
#endif

namespace Nebula::ndx
{
    inline std::string to_string(const Format format)
    {
        switch (format)
        {
            using enum Format;
            case eUnknown:              return "Format::Unknown";
            case eR8Srgb:               return "Format::R8Srgb";
            case eR8Unorm:              return "Format::R8Unorm";
            case eR8G8Srgb:             return "Format::R8G8Srgb";
            case eR8G8Unorm:            return "Format::R8G8Unorm";
            case eR8G8B8Srgb:           return "Format::R8G8B8Srgb";
            case eR8G8B8Unorm:          return "Format::R8G8B8Unorm";
            case eR8G8B8A8Srgb:         return "Format::R8G8B8A8Srgb";
            case eR8G8B8A8Unorm:        return "Format::R8G8B8A8Unorm";
            case eB8G8R8A8Unorm:        return "Format::B8G8R8A8Unorm";
            case eB8G8R8A8Srgb:         return "Format::B8G8R8A8Srgb";
            case eR16Sfloat:            return "Format::R16Sfloat";
            case eR16G16Sfloat:         return "Format::R16G16Sfloat";
            case eR16G16B16Sfloat:      return "Format::R16G16B16Sfloat";
            case eR16G16B16A16Sfloat:   return "Format::R16G16B16A16Sfloat";
            case eR32Sfloat:            return "Format::R32Sfloat";
            case eR32G32Sfloat:         return "Format::R32G32Sfloat";
            case eR32G32B32Sfloat:      return "Format::R32G32B32Sfloat";
            case eR32G32B32A32Sfloat:   return "Format::R32G32B32A32Sfloat";
            case eD16Unorm:             return "Format::D16Unorm";
            case eX8D24UnormPack32:     return "Format::X8D24UnormPack32";
            case eD32Sfloat:            return "Format::D32Sfloat";
            case eS8Uint:               return "Format::S8Uint";
            case eD16UnormS8Uint:       return "Format::D16UnormS8Uint";
            case eD24UnormS8Uint:       return "Format::D24UnormS8Uint";
            case eD32SfloatS8Uint:      return "Format::D32SfloatS8Uint";
            default:                    return "Format::Unknown";
        }
    }

    inline std::string to_string(const VertexInputRate vertex_input_rate)
    {
        switch (vertex_input_rate)
        {
            enum2str_case(VertexInputRate::eInstance);
            enum2str_case(VertexInputRate::eVertex);
        }
    }

    inline std::string to_string(const PipelineType type)
    {
        switch (type)
        {
            enum2str_case(PipelineType::eCompute);
            enum2str_case(PipelineType::eGraphics);
            enum2str_case(PipelineType::eRayTracing);
            enum2str_case(PipelineType::eUnknown);
        }
    }
}