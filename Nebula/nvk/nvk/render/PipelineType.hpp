#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

namespace Nebula::nvk
{
    enum class PipelineType
    {
        eCompute,
        eGraphics,
        eRayTracing,
        eUnknown,
    };

    inline std::string to_string(const PipelineType type)
    {
        using enum PipelineType;
        switch (type)
        {
            case eCompute:      return "Compute";
            case eGraphics:     return "Graphics";
            case eRayTracing:   return "Ray Tracing";
            default:            return "Unknown";
        }
    }

    inline vk::PipelineBindPoint to_bind_point(const PipelineType type)
    {
        using enum PipelineType;
        switch (type)
        {
            case eCompute:      return vk::PipelineBindPoint::eCompute;
            case eGraphics:     return vk::PipelineBindPoint::eGraphics;
            case eRayTracing:   return vk::PipelineBindPoint::eRayTracingKHR;
            default:            throw std::invalid_argument("No appropriate bind point for pipeline of Unknown type");
        }
    }
}