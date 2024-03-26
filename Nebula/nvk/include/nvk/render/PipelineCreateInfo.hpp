#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "../Device.hpp"
#include "../Utility.hpp"
#include "PipelineState.hpp"
#include "PipelineType.hpp"
#include "Shader.hpp"
#include "VertexInput.hpp"

namespace Nebula::nvk
{
    class PipelineCreateInfo
    {
    public:
        NVK_DISABLE_COPY(PipelineCreateInfo);

        PipelineCreateInfo() = default;

        template <VertexInput T>
        PipelineCreateInfo& add_attribute_descriptions(uint32_t base_location = 0, uint32_t binding = 0)
        {
            for (const auto& attribute : T::attribute_descriptions(base_location, binding))
            {
                m_pipeline_state.attribute_descriptions.push_back(attribute);
            }
            return *this;
        }

        template <VertexInput T>
        PipelineCreateInfo& add_binding_description(uint32_t binding = 0)
        {
            for (const auto& b : T::binding_description(binding))
            {
                m_pipeline_state.binding_descriptions.push_back(b);
            }
            return *this;
        }

        PipelineCreateInfo& set_pipeline_type(PipelineType type);

        PipelineCreateInfo& add_descriptor_set_layout(const vk::DescriptorSetLayout& descriptor_set_layout);

        PipelineCreateInfo& add_push_constant(const vk::PushConstantRange& push_constant);

        PipelineCreateInfo& add_shader(const std::string& shader_src,
                                       vk::ShaderStageFlagBits shader_stage,
                                       const std::string& entry_point = "main");

        PipelineCreateInfo& configure_state(const std::function<void(PipelineState&)>& lambda);

        PipelineCreateInfo& add_attachment(bool enable_blending = false);

        [[deprecated("Maybe don't use this 💀")]]
        PipelineCreateInfo& set_attachment_count(uint32_t value);

        PipelineCreateInfo& set_cull_mode(vk::CullModeFlagBits cull_mode);

        PipelineCreateInfo& set_name(const std::string& name);

        PipelineCreateInfo& set_ray_recursion_depth(uint32_t depth = 1);

        PipelineCreateInfo& set_render_pass(const vk::RenderPass& render_pass);

        PipelineCreateInfo& set_sample_count(vk::SampleCountFlagBits sample_count);

        PipelineCreateInfo& set_wireframe_mode(bool value = true);

    private:
        std::vector<vk::DescriptorSetLayout>    m_descriptor_set_layouts {};
        std::string                             m_name {"Unknown"};
        PipelineState                           m_pipeline_state {};
        std::vector<vk::PushConstantRange>      m_push_constants {};
        uint32_t                                m_ray_depth {1};
        vk::RenderPass                          m_render_pass {};
        vk::SampleCountFlagBits                 m_sample_count {vk::SampleCountFlagBits::e1};
        std::vector<ShaderCreateInfo>           m_shader_sources {};
        PipelineType                            m_type {PipelineType::eUnknown};
        bool                                    m_with_render_pass;

        friend class Pipeline;
    };
}