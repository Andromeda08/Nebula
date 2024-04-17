#pragma once

#include <vulkan/vulkan.hpp>
#include "../Buffer.hpp"
#include "../Device.hpp"
#include "../Utility.hpp"
#include "../render/Pipeline.hpp"

namespace Nebula::nvk
{
    struct ShaderBindingTableCreateInfo
    {
        struct_param(uint32_t, miss_count, 0);
        struct_param(uint32_t, hit_count, 0);
        struct_param(uint32_t, callable_count, 0);
        struct_param(std::string, name, {});
        struct_param(vk::Pipeline, pipeline, {});
    };

    class ShaderBindingTable
    {
    public:
        NVK_DISABLE_COPY(ShaderBindingTable);

        ShaderBindingTable(const ShaderBindingTableCreateInfo& create_info,
                           const std::shared_ptr<Device>&      device);

        const std::shared_ptr<Buffer>& sbt() const { return m_buffer; }

        const vk::StridedDeviceAddressRegionKHR* rgen_region() const { return &m_rgen; }
        const vk::StridedDeviceAddressRegionKHR* miss_region() const { return &m_miss; }
        const vk::StridedDeviceAddressRegionKHR* hit_region() const  { return &m_hit; }
        const vk::StridedDeviceAddressRegionKHR* call_region() const { return &m_callable; }

        static std::shared_ptr<ShaderBindingTable> create(const ShaderBindingTableCreateInfo& create_info,
                                                          const std::shared_ptr<Device>&      device)
        {
            return std::make_shared<ShaderBindingTable>(create_info, device);
        }

    private:
        void get_ray_tracing_properties(vk::PhysicalDeviceRayTracingPipelinePropertiesKHR* p_rt_props);

        /**
         * Round up sizes to next alignment
         * https://github.com/nvpro-samples/nvpro_core/blob/master/nvh/alignment.hpp
         */
        template <class Integral>
        constexpr Integral align_up(Integral x, size_t a) noexcept
        {
            return Integral((x + (Integral(a) - 1)) & ~Integral(a - 1));
        }

        std::shared_ptr<Device> m_device;
        std::shared_ptr<Buffer> m_buffer;
        const vk::Pipeline&     m_pipeline;

        uint32_t m_miss_count {0};
        uint32_t m_hit_count {0};
        uint32_t m_callable_count {0};

        vk::StridedDeviceAddressRegionKHR m_rgen {};
        vk::StridedDeviceAddressRegionKHR m_miss {};
        vk::StridedDeviceAddressRegionKHR m_hit {};
        vk::StridedDeviceAddressRegionKHR m_callable {};
    };
}