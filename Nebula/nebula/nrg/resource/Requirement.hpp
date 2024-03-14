#pragma once

#include <string>
#include <vulkan/vulkan.hpp>
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    struct Requirement
    {
        std::string     name    {"Unknown Resource"};
        ResourceUsage   usage   {ResourceUsage::eUnknown};
        ResourceType    type    {ResourceType::eUnknown};
        bool            enabled {true};

        Requirement() = default;

        Requirement(std::string _name, ResourceUsage _usage, ResourceType _type)
        : name(std::move(_name)), usage(_usage), type(_type) {}

        template <typename T>
        T& as()
        {
            static_assert(std::is_base_of_v<Requirement, T>, "Template parameter T must be a valid Resource Requirement");
            return dynamic_cast<T&>(*this);
        }

        virtual ~Requirement() = default;
    };

    struct ImageRequirement : public Requirement
    {
        using enum vk::ImageUsageFlagBits;
        using enum vk::Format;

        vk::Format          format      {eR32G32B32A32Sfloat};
        vk::Extent2D        extent      {0, 0};
        vk::ImageUsageFlags usage_flags {eTransferSrc | eSampled | eStorage};

        ImageRequirement() = default;

        ImageRequirement(std::string _name, ResourceUsage _usage, ResourceType _type,
                         vk::Format _format               = eR32G32B32A32Sfloat,
                         vk::Extent2D _extent             = {0, 0},
                         vk::ImageUsageFlags _image_usage = {eTransferSrc | eSampled | eStorage})
        : Requirement(std::move(_name), _usage, _type), format(_format), extent(_extent), usage_flags(_image_usage) {}

        ~ImageRequirement() override = default;
    };

    struct BufferRequirement : public Requirement
    {
        using enum vk::BufferUsageFlagBits;

        vk::BufferUsageFlags usage_flags {eStorageBuffer};

        BufferRequirement() = default;

        BufferRequirement(std::string _name, ResourceUsage _usage, ResourceType _type,
                          vk::BufferUsageFlags _usage_flags = eStorageBuffer)
        : Requirement(std::move(_name), _usage, _type), usage_flags(_usage_flags) {}

        ~BufferRequirement() override = default;
    };
}