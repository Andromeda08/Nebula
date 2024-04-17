#pragma once

#include <string>
#include <memory>
#include <vulkan/vulkan.hpp>
#include <nrg/common/Context.hpp>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/common/ResourceTraits.hpp>
#include <nrg/resource/Resource.hpp>

namespace Nebula::nrg
{
    struct ResourceCreateInfo
    {
        ResourceClaim         claim;
        vk::Format            format;
        std::string           name;
        ResourceType          type;
        vk::ImageUsageFlags   usage_flags;
    };

    class ResourceFactory
    {
    public:
        explicit ResourceFactory(const std::shared_ptr<Context>& context);

        std::shared_ptr<Resource> create(const ResourceCreateInfo& create_info);

    private:
        std::shared_ptr<Context> m_context;
    };
}