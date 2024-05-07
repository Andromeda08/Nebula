#pragma once

#include <cstdint>
#include <memory>
#include <set>
#include <tuple>
#include <variant>
#include <vulkan/vulkan.hpp>
#include "DeviceExtensions.hpp"
#include "Utility.hpp"
#include "Queue.hpp"

namespace Nebula::nvk
{
    struct AllocationInfo
    {
        vk::MemoryPropertyFlags             property_flags;
        std::variant<vk::Buffer, vk::Image> target;

        inline AllocationInfo& set_property_flags(vk::MemoryPropertyFlags value)
        {
            property_flags = value;
            return *this;
        }

        inline AllocationInfo& set_image(const vk::Image& value)
        {
            target = value;
            return *this;
        }

        inline AllocationInfo& set_buffer(const vk::Buffer& value)
        {
            target = value;
            return *this;
        }
    };

    class Allocation
    {
    public:
        Allocation(const std::variant<vk::Buffer, vk::Image>& user, const vk::DeviceSize& device_size,
                   const vk::Device& device, uint32_t id);

        void bind();

        void* map();

        void unmap();

        void free();

        ~Allocation()
        {
            if (!m_invalid)
            {
                free();
            }
        }

        vk::DeviceMemory                          memory {};
        const vk::DeviceSize                      size {};
        const vk::DeviceSize                      offset {0};

    private:
        const uint32_t                            m_id;
        bool                                      m_invalid {false};
        const std::variant<vk::Buffer, vk::Image> m_user;
        const vk::Device&                         m_device;
    };

    struct MemoryUsage
    {
        float budget;
        std::string budget_coefficient;
        float usage;
        std::string usage_coefficient;
    };

    class Device
    {
    public:
        NVK_DISABLE_COPY(Device);

        explicit Device(const vk::Instance& instance, const DeviceExtensions& device_extensions);

        std::shared_ptr<Allocation> allocate_memory(const AllocationInfo& allocation_info);

        void name_object(const std::string& name, uint64_t handle, vk::ObjectType type) const;

        template <typename T>
        void name_object(const T& handle, const std::string& name, vk::ObjectType type) const
        {
            name_object(name, uint64_t(static_cast<T::CType>(handle)), type);
        }

        void wait_idle() const { m_device.waitIdle(); }

        const std::shared_ptr<Queue>& q_general() const { return m_queue_general; }

        const std::shared_ptr<Queue>& q_async_compute() const { return m_queue_async_compute; }

        const vk::PhysicalDevice& physical_device() const noexcept { return m_physical_device; }

        const vk::Device& handle() const noexcept { return m_device; }

        bool is_raytracing_enabled() const noexcept { return m_device_extensions.rt_pipeline.rayTracingPipeline; }

        MemoryUsage get_memory_usage() const;

    private:
        void select_device(const vk::Instance& instance);

        void create_device();

        uint32_t find_memory_heap_index(uint32_t filter, vk::MemoryPropertyFlags property_flags) const;

        std::tuple<float, std::string> _convert_memory_size(uint64_t input_memory) const;

        vk::PhysicalDevice              m_physical_device;
        vk::PhysicalDeviceProperties    m_physical_device_properties;
        vk::Device                      m_device;
        std::shared_ptr<Queue>          m_queue_general;
        std::shared_ptr<Queue>          m_queue_async_compute;
        const DeviceExtensions          m_device_extensions;

        std::vector<std::shared_ptr<Allocation>> m_allocations;
    };
}