#include "Queue.hpp"
#include <optional>
#include <nlog/nlog.hpp>

#if defined(NVK_VERBOSE) || defined(NVK_VERBOSE_EXTRA)
#include <iostream>
#endif


namespace Nebula::nvk
{
    auto Queue::find_queue(const vk::PhysicalDevice& device,
                           vk::QueueFlags required_flags,
                           vk::QueueFlags excluded_flags,
                           bool required) -> std::optional<QueueFamilyProperties>
    {
        const std::vector<vk::QueueFamilyProperties> queue_families = device.getQueueFamilyProperties();
        const auto it = std::ranges::find_if(queue_families,[required_flags, excluded_flags](const vk::QueueFamilyProperties& properties){
            return (properties.queueCount > 0)
                   && (properties.queueFlags & required_flags)
                   && !(properties.queueFlags & excluded_flags);
        });

        const bool has_queue = (it != std::end(queue_families));
        if (required && !has_queue)
        {
            throw nlog::make_exception("No Queue Family found with required flags [{}] and excluded flags [{}]",
                                       to_string(required_flags), to_string(excluded_flags));
        }
        const uint32_t family_index = static_cast<uint32_t>(it - std::begin(queue_families));

        #ifdef NVK_VERBOSE_EXTRA
        if (required)
        {
            std::cout << nlog::fmt_verbose("Found Queue Family [Index: {}] with required flags [{}] and excluded flags [{}]",
                                           family_index, to_string(required_flags), to_string(excluded_flags)) << std::endl;
        }
        #endif

        return (has_queue)
               ? std::make_optional(QueueFamilyProperties { *it, family_index })
               : std::nullopt;
    }

    auto Queue::has_queue(const vk::PhysicalDevice& device, vk::QueueFlags required, vk::QueueFlags excluded) -> bool
    {
        return find_queue(device, required, excluded).has_value();
    }

    Queue::Queue(const QueueCreateInfo& create_info, const vk::Device& device)
    : family_index(create_info.family_index)
    , index(create_info.queue_index)
    , name(create_info.name)
    {
        device.getQueue(family_index, index, &queue);

        #ifdef NVK_VERBOSE
        std::cout << nlog::fmt_info("Created {} with name \"{}\", #{} on Queue Family {} ", nlog::cyan("vk::Queue"), name, index, family_index) << std::endl;
        #endif
    }
}