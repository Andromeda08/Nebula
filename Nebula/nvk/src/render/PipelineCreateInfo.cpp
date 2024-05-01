#include "render/PipelineCreateInfo.hpp"

namespace Nebula::nvk
{
    PipelineCreateInfo& PipelineCreateInfo::set_pipeline_type(const PipelineType type)
    {
        m_type = type;
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::add_descriptor_set_layout(const vk::DescriptorSetLayout& descriptor_set_layout)
    {
        m_descriptor_set_layouts.push_back(descriptor_set_layout);
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::add_shader(const std::string& shader_src,
                                                       vk::ShaderStageFlagBits shader_stage,
                                                       const std::string& entry_point)
    {
        auto create_info = ShaderCreateInfo()
            .set_entry_point(entry_point)
            .set_file_path(shader_src)
            .set_shader_stage(shader_stage);

        m_shader_sources.push_back(create_info);
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::add_push_constant(const vk::PushConstantRange& push_constant)
    {
        m_push_constants.push_back(push_constant);
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::configure_state(const std::function<void(PipelineState&)>& lambda)
    {
        lambda(m_pipeline_state);
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::add_attachment(bool enable_blending)
    {
        auto attachment = PipelineState::make_color_blend_attachment_state();
        if (enable_blending)
        {
            attachment
                .setBlendEnable(true)
                .setDstAlphaBlendFactor(vk::BlendFactor::eOne);
        }
        m_pipeline_state.attachment_states.push_back(attachment);
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::add_attachment(const vk::PipelineColorBlendAttachmentState& blend_state)
    {
        m_pipeline_state.attachment_states.push_back(blend_state);
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::set_attachment_count(uint32_t value)
    {
        m_pipeline_state.attachment_states.clear();
        for (uint32_t i = 0; i < value; i++)
        {
            m_pipeline_state.attachment_states.push_back(PipelineState::make_color_blend_attachment_state());
        }
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::set_cull_mode(vk::CullModeFlagBits cull_mode)
    {
        m_pipeline_state.rasterization_state.setCullMode(cull_mode);
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::set_name(const std::string& name)
    {
        m_name = name;
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::set_ray_recursion_depth(uint32_t depth)
    {
        m_ray_depth = depth;
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::set_render_pass(const vk::RenderPass& render_pass)
    {
        m_render_pass = render_pass;
        m_with_render_pass = true;
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::set_sample_count(vk::SampleCountFlagBits sample_count)
    {
        m_pipeline_state.multisample_state.setRasterizationSamples(sample_count);
        m_sample_count = sample_count;
        return *this;
    }

    PipelineCreateInfo& PipelineCreateInfo::set_wireframe_mode(bool value)
    {
        m_pipeline_state.rasterization_state.setPolygonMode(vk::PolygonMode::eLine);
        return *this;
    }
}