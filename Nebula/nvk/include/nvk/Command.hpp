#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "Device.hpp"
#include "Utility.hpp"

#ifdef NVK_VERBOSE
#include <format>
#include <iostream>
#endif

namespace Nebula::nvk
{
    class CommandRing : public Ring<vk::CommandBuffer>
    {
    public:
        NVK_DISABLE_COPY(CommandRing);

        CommandRing(uint32_t count, const vk::CommandPool& command_pool, const vk::Device& device, const std::shared_ptr<Queue>& queue)
        : Ring<vk::CommandBuffer>(count), m_queue(queue), m_device(device), m_pool(command_pool)
        {
            auto allocate_info = vk::CommandBufferAllocateInfo()
                .setCommandBufferCount(count)
                .setCommandPool(m_pool)
                .setLevel(vk::CommandBufferLevel::ePrimary);
            const vk::Result result = device.allocateCommandBuffers(&allocate_info, this->underlying_data());

            #ifdef NVK_VERBOSE
            std::cout << std::format("[V] Created a CommandRing of size {} on Queue Family {}", this->size(), m_queue->family_index) << std::endl;
            #endif
        }

        ~CommandRing() override
        {
            m_device.freeCommandBuffers(m_pool, this->size(), this->underlying_data());

            #ifdef NVK_VERBOSE
            std::cout << std::format("[V] Destroyed a CommandRing of size {} on Queue Family {}", this->size(), m_queue->family_index) << std::endl;
            #endif
        }

    private:
        std::shared_ptr<Queue>  m_queue;
        const vk::Device&       m_device;
        const vk::CommandPool&  m_pool;
    };

    class CommandPool
    {
    public:
        NVK_DISABLE_COPY(CommandPool);

        explicit CommandPool(const std::shared_ptr<Device>& device);

        std::shared_ptr<CommandRing> create_command_ring(uint32_t count, const std::shared_ptr<Queue>& queue)
        {
            return std::make_shared<CommandRing>(count, get_pool(queue->family_index), m_device->handle(), queue);
        }

        void exec_single_time_command(const std::function<void(const vk::CommandBuffer&)>& commands,
                                      std::optional<uint32_t> queue_family = std::nullopt);

    private:
        const vk::CommandPool& get_pool(uint32_t family_index);

        const vk::Queue& get_queue(uint32_t family_index);

        uint32_t                                    m_default_queue_family;
        std::shared_ptr<Device>                     m_device;
        std::map<uint32_t, vk::CommandPool>         m_pools;
        std::map<uint32_t, std::shared_ptr<Queue>>  m_queues;
    };
}