#pragma once

#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "../Buffer.hpp"
#include "../Command.hpp"
#include "../Device.hpp"
#include "../Utility.hpp"

namespace Nebula::nvk
{
    struct TLASInstanceInfo
    {
        vk::DeviceAddress       blas_address;
        uint32_t                hit_group {0};
        uint32_t                mask {0xFF};
        vk::TransformMatrixKHR  transform;
    };

    struct TLASCreateInfo
    {
        const std::vector<TLASInstanceInfo>& instance_info;
        std::string name;
    };

    struct TLASUpdateInfo
    {
        const std::vector<TLASInstanceInfo>& instance_info;
    };

    class TLAS
    {
    public:
        NVK_DISABLE_COPY(TLAS);

        TLAS(const TLASCreateInfo& create_info,
             const std::shared_ptr<Device>& device,
             const std::shared_ptr<CommandPool>& command_pool);

        void update(const TLASUpdateInfo& update_info);

        void update(const TLASUpdateInfo& update_info, const vk::CommandBuffer& command_buffer);

        const vk::AccelerationStructureKHR& handle() const { return m_tlas; }

    private:
        void build(const std::vector<TLASInstanceInfo>& geometry_info);

        vk::AccelerationStructureKHR m_tlas;
        std::shared_ptr<Buffer>      m_buffer;
        std::shared_ptr<Buffer>      m_instance_data;
        uint32_t                     m_instance_count {0};
        std::shared_ptr<Device>      m_device;
        std::shared_ptr<CommandPool> m_command_pool;
        std::string                  m_name;
    };
}