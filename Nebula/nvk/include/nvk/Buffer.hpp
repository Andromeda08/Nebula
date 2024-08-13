#pragma once

#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>
#include "Command.hpp"
#include "Device.hpp"
#include "Image.hpp"
#include "Utility.hpp"

namespace Nebula::nvk
{
    enum class BufferType
    {
        eCustom,
        eAccelerationStructureStorage,
        eIndex,
        eShaderBindingTable,
        eStaging,
        eStorage,
        eUniform,
        eVertex,
        eUnknown
    };

    std::string to_string(BufferType buffer_type);

    struct BufferCreateInfo
    {
        vk::DeviceSize          size {0};
        vk::MemoryPropertyFlags extra_memory_property_flags {};
        std::string             name {"Unknown"};
        BufferType              type {BufferType::eUnknown};
        vk::BufferUsageFlags    extra_usage_flags {};

        BufferCreateInfo() = default;

        inline BufferCreateInfo& add_memory_property_flags(vk::MemoryPropertyFlags value)
        {
            extra_memory_property_flags = value;
            return *this;
        }

        inline BufferCreateInfo& add_usage_flags(vk::BufferUsageFlags value)
        {
            extra_usage_flags = value;
            return *this;
        }

        inline BufferCreateInfo& set_size(vk::DeviceSize value)
        {
            size = value;
            return *this;
        }

        inline BufferCreateInfo& set_name(const std::string& value)
        {
            name = value;
            return *this;
        }

        inline BufferCreateInfo& set_buffer_type(BufferType value)
        {
            type = value;
            return *this;
        }
    };

    class Buffer
    {
    public:
        NVK_DISABLE_COPY(Buffer);

        Buffer(const BufferCreateInfo& create_info, const std::shared_ptr<Device>& device);

        ~Buffer();

        static inline std::shared_ptr<Buffer> create(const BufferCreateInfo& create_info,
                                                     const std::shared_ptr<Device>& device)
        {
            return std::make_shared<Buffer>(create_info, device);
        }

        template <typename T>
        static std::shared_ptr<Buffer> create_with_data(const BufferCreateInfo& create_info,
                                                        T* p_data,
                                                        const std::shared_ptr<Device>& device,
                                                        const std::shared_ptr<CommandPool>& command_pool)
        {
            auto buffer = std::make_shared<Buffer>(create_info, device);
            auto staging_create_info = BufferCreateInfo()
                .set_buffer_type(BufferType::eStaging)
                .set_name(create_info.name)
                .set_size(create_info.size);
            auto staging = std::make_shared<Buffer>(staging_create_info, device);
            staging->set_data(p_data);
            command_pool->exec_single_time_command([&](const vk::CommandBuffer& command_buffer){
                staging->copy_to_buffer(*buffer, command_buffer);
            });
            return buffer;
        }

        template <typename T>
        void set_data(T* p_data)
        {
            if (sizeof(T) > size())
            {
                std::string message = "Tried to copy data of greater size than the memory of buffer";
                #ifdef NVK_VERBOSE
                message = fmt::format("Tried to copy data of greater size ({} > {}) than the memory of buffer \"{}\"",
                                      sizeof(T), size(), m_name);
                #endif
                throw std::runtime_error(message);
            }

            void* mapped_memory = m_allocation->map();
            std::memcpy(mapped_memory, p_data, static_cast<size_t>(size()));
            m_allocation->unmap();
        }

        void copy_to_buffer(const Buffer& dst, const vk::CommandBuffer& command_buffer);

        void copy_to_image(const Image& dst, const vk::CommandBuffer& command_buffer);

        const vk::DeviceAddress& address() const { return m_address; }

        const vk::Buffer& buffer() const { return m_buffer; }

        const vk::DeviceSize& size() const { return m_allocation->size; }

        const vk::DeviceSize& offset() const { return m_allocation->offset; }

    private:
        struct BufferTypeFlags
        {
            vk::MemoryPropertyFlags memory_flags;
            vk::BufferUsageFlags    usage_flags;

            static BufferTypeFlags for_type(BufferType buffer_type);
        };

        std::shared_ptr<Allocation> m_allocation;
        vk::DeviceAddress           m_address;
        vk::Buffer                  m_buffer;

        std::shared_ptr<Device>     m_device;
        const std::string           m_name;
        const BufferType            m_type;

        friend class ShaderBindingTable;
    };
}