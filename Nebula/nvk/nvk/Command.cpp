#include "Command.hpp"
#include <nlog/nlog.hpp>

namespace Nebula::nvk
{
    CommandPool::CommandPool(const std::shared_ptr<Device>& device)
    : m_default_queue_family(device->q_general()->family_index)
    , m_device(device)
    {
        std::vector<uint32_t> q_families = { device->q_general()->family_index, device->q_async_compute()->family_index };
        for (const uint32_t fidx : q_families)
        {
            vk::CommandPool pool;
            auto create_info = vk::CommandPoolCreateInfo()
                .setQueueFamilyIndex(fidx)
                .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

            if (const vk::Result result = m_device->handle().createCommandPool(&create_info, nullptr, &pool);
                result != vk::Result::eSuccess)
            {
                throw nlog::make_exception("Failed to create vk::CommandPool on Queue Family {} ({})",fidx, to_string(result));
            }

            #ifdef NVK_VERBOSE
            std::cout << nlog::fmt_info("Created vk::CommandPool on Queue Family {}", fidx) << std::endl;
            #endif

            m_pools.insert({ fidx, pool });
        }

        m_queues.insert({ q_families[0], device->q_general() });
        m_queues.insert({ q_families[1], device->q_async_compute() });
    }

    void CommandPool::exec_single_time_command(const std::function<void(const vk::CommandBuffer&)>& commands, std::optional<uint32_t> queue_family)
    {
        uint32_t family_index = m_default_queue_family;
        if (queue_family.has_value())
        {
            family_index = queue_family.value();
        }

        const auto& pool = get_pool(family_index);

        auto allocate_info = vk::CommandBufferAllocateInfo()
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandPool(pool)
            .setCommandBufferCount(1);

        vk::CommandBuffer buffer;
        if (const vk::Result result = m_device->handle().allocateCommandBuffers(&allocate_info, &buffer);
            result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to allocate vk::CommandBuffer for single time use on Queue Family {}", family_index);
        }

        auto begin_info = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        if (const vk::Result result = buffer.begin(&begin_info); result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to begin vk::CommandBuffer for single time use on Queue Family {}", family_index);
        }

        commands(buffer);

        buffer.end();

        const auto& queue = get_queue(family_index);
        auto submit_info = vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&buffer);
        if (const vk::Result result = queue.submit(1, &submit_info, nullptr); result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to submit vk::CommandBuffer for single time use on Queue Family {}", family_index);
        }

        queue.waitIdle();
        m_device->handle().freeCommandBuffers(pool, 1, &buffer);
    }

    const vk::CommandPool& CommandPool::get_pool(uint32_t family_index)
    {
        if (m_pools.contains(family_index))
        {
            return m_pools.at(family_index);
        }

        throw nlog::make_exception<std::out_of_range>("No CommandPool found for Queue Family with index {}", family_index);
    }

    const vk::Queue& CommandPool::get_queue(uint32_t family_index)
    {
        if (m_queues.contains(family_index))
        {
            return m_queues.at(family_index)->queue;
        }

        throw nlog::make_exception<std::out_of_range>("No Queue found for Queue Family with index {}", family_index);
    }
}