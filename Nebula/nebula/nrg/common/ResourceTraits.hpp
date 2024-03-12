#pragma once

#include <format>
#include <stdexcept>
#include <string>
#include <glm/vec4.hpp>

namespace Nebula::nrg
{
    enum class ResourceLifetime
    {
        eTransient,
        eExternal,
        eUnknown,
    };

    enum class ResourceType
    {
        eImage,
        eImageArray,
        eStorageBuffer,
        eSceneData,
        eUnknown
    };

    enum class ResourceUsage
    {
        eInput,
        eOutput,
        eUnknown
    };


    // Enum to String conversion methods ----------------------
    inline std::string to_string(const ResourceLifetime resource_lifetime)
    {
        using enum ResourceLifetime;
        switch (resource_lifetime)
        {
            case eExternal:  return "External";
            case eTransient: return "Transient";
            default:         return "Unknown";
        }
    }

    inline std::string to_string(const ResourceType resource_type)
    {
        using enum ResourceType;
        switch (resource_type)
        {
            case eImage:         return "Image";
            case eImageArray:    return "ImageArray";
            case eSceneData:     return "SceneData";
            case eStorageBuffer: return "StorageBuffer";
            default:             return "Unknown";
        }
    }

    inline std::string to_string(const ResourceUsage resource_usage)
    {
        using enum ResourceUsage;
        switch (resource_usage)
        {
            case eInput:   return "Input";
            case eOutput:  return "Write";
            default:       return "Unknown";
        }
    }

    // String to Enum conversion methods ----------------------
    inline ResourceType to_resource_type(const std::string& str)
    {
        using enum ResourceType;

        if (str == "Image")         return eImage;
        if (str == "ImageArray")    return eImageArray;
        if (str == "SceneData")     return eSceneData;
        if (str == "StorageBuffer") return eStorageBuffer;
        if (str == "Unknown")       return eUnknown;

        throw std::runtime_error(std::format(R"(Unknown ResourceType "{}")", str));
    }

    // Other enum based conversion methods --------------------
    inline glm::ivec4 get_resource_color(const ResourceType resource_type)
    {
        using enum ResourceType;
        switch (resource_type)
        {
            case eImage:            return {  59, 130, 246, 255 };
            case eImageArray:       return {  20, 184, 166, 255 };
            case eStorageBuffer:    return { 251, 191,  36, 255 };
            case eSceneData:        return {  14, 165, 233, 255 };
            default:                return { 128, 128, 128, 255 };
        }
    }
}