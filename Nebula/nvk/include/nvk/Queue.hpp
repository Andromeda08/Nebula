#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace Nebula::nvk
{
    struct QueueFamilyProperties
    {
        vk::QueueFamilyProperties vk_qf_properties;
        uint32_t                  family_index;
    };

    struct QueueCreateInfo
    {
        uint32_t    family_index {0};
        uint32_t    queue_index {0};
        std::string name {"Unknown Queue"};

        QueueCreateInfo() = default;

        inline QueueCreateInfo& set_queue_family_index(uint32_t value)
        {
            family_index = value;
            return *this;
        }

        inline QueueCreateInfo& set_queue_index(uint32_t value)
        {
            queue_index = value;
            return *this;
        }

        inline QueueCreateInfo& set_name(const std::string& value)
        {
            name = value;
            return *this;
        }
    };

    struct Queue
    {
        const bool          can_present {false};
        const uint32_t      family_index {0};
        const uint32_t      index {0};
        const std::string   name {"Unknown Queue"};
        vk::Queue           queue {};

        Queue(const QueueCreateInfo& create_info, const vk::Device& device);

        static std::optional<QueueFamilyProperties> find_queue(const vk::PhysicalDevice& physical_device,
                                                               vk::QueueFlags required_flags,
                                                               vk::QueueFlags excluded_flags = {},
                                                               bool required = false);

        static bool has_queue(const vk::PhysicalDevice& physical_device,
                              vk::QueueFlags required,
                              vk::QueueFlags excluded = {});
    };
}