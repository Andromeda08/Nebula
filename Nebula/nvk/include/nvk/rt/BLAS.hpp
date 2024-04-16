#pragma once

#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>
#include "../Buffer.hpp"
#include "../Command.hpp"
#include "../Device.hpp"
#include "../Utility.hpp"

namespace Nebula::nvk
{
    struct BLASCreateInfo
    {
        std::shared_ptr<Buffer> vertex_buffer;
        uint32_t                vertex_count {0};
        std::shared_ptr<Buffer> index_buffer;
        uint32_t                index_count {0};
        size_t                  vertex_stride {0};
        std::string             name {"Unknown"};
    };

    class BLAS
    {
    public:
        NVK_DISABLE_COPY(BLAS);

        BLAS(const BLASCreateInfo& create_info,
             const std::shared_ptr<Device>& device,
             const std::shared_ptr<CommandPool>& command_pool);

        const vk::AccelerationStructureKHR& handle() const { return m_blas; }

        vk::DeviceAddress address() const { return m_address; }

        static inline std::shared_ptr<BLAS> create(const BLASCreateInfo& create_info,const std::shared_ptr<Device>& device,
                                                   const std::shared_ptr<CommandPool>& command_pool)
        {
            return std::make_shared<BLAS>(create_info, device, command_pool);
        }

    private:
        vk::AccelerationStructureKHR    m_blas;
        vk::DeviceAddress               m_address;
        std::shared_ptr<Buffer>         m_buffer;
        std::shared_ptr<Device>         m_device;
        const BLASCreateInfo            m_create_info;
    };
}