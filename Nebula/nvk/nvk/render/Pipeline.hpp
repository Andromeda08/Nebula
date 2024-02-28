#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include "../Device.hpp"
#include "../Utility.hpp"
#include "PipelineCreateInfo.hpp"
#include "PipelineState.hpp"
#include "PipelineType.hpp"

namespace Nebula::nvk
{
    class Pipeline
    {
    public:
        NVK_DISABLE_COPY(Pipeline);

        Pipeline(PipelineCreateInfo& create_info, const std::shared_ptr<Device>& device);

        void bind(const vk::CommandBuffer& command_buffer);

        ~Pipeline();

    private:
        void create_pipeline_layout(const PipelineCreateInfo& create_info);

        void create_compute(const PipelineCreateInfo& create_info);

        void create_graphics(const PipelineCreateInfo& create_info);

        void create_ray_tracing(const PipelineCreateInfo& create_info);

        std::vector<vk::RayTracingShaderGroupCreateInfoKHR> create_rt_shader_groups(const std::vector<vk::PipelineShaderStageCreateInfo>& shader_stages);

        vk::Pipeline                        m_pipeline {};
        vk::PipelineLayout                  m_pipeline_layout {};
        std::shared_ptr<Device>             m_device;
        PipelineState                       m_create_info {};
        const std::string                   m_name {"Unknown"};
        const PipelineType                  m_type {PipelineType::eUnknown};
        const vk::PipelineBindPoint         m_bind_point;
        // std::shared_ptr<ShaderBindingTable> m_sbt;
    };
}