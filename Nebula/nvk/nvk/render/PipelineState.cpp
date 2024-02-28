#include "PipelineState.hpp"

namespace Nebula::nvk
{
    PipelineState::PipelineState()
    {
        set_input_assembly_state();
        set_rasterization_state();
        set_multisample_state();
        set_depth_stencil_state();
        set_viewport_state();
        set_dynamic_state();
        set_color_blend_state();
        set_vertex_input_state();
    }

    void PipelineState::update()
    {
        color_blend_state.setAttachmentCount(static_cast<uint32_t>(attachment_states.size()));
        color_blend_state.setPAttachments(attachment_states.data());

        vertex_input_state.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attribute_descriptions.size()));
        vertex_input_state.setPVertexAttributeDescriptions(attribute_descriptions.data());

        vertex_input_state.setVertexBindingDescriptionCount(static_cast<uint32_t>(binding_descriptions.size()));
        vertex_input_state.setPVertexBindingDescriptions(binding_descriptions.data());

        dynamic_state.setDynamicStateCount(static_cast<uint32_t>(dynamic_states.size()));
        dynamic_state.setPDynamicStates(dynamic_states.data());
    }

    void PipelineState::set_input_assembly_state()
    {
        input_assembly_state = vk::PipelineInputAssemblyStateCreateInfo()
            .setTopology(vk::PrimitiveTopology::eTriangleList)
            .setPrimitiveRestartEnable(false)
            .setPNext(nullptr);
    }

    void PipelineState::set_rasterization_state()
    {
        rasterization_state = vk::PipelineRasterizationStateCreateInfo()
            .setPolygonMode(vk::PolygonMode::eFill)
            .setCullMode(vk::CullModeFlagBits::eBack)
            .setFrontFace(vk::FrontFace::eCounterClockwise)
            .setDepthClampEnable(false)
            .setDepthBiasEnable(false)
            .setDepthBiasClamp(0.0f)
            .setDepthBiasSlopeFactor(0.0f)
            .setLineWidth(1.0f)
            .setRasterizerDiscardEnable(false)
            .setPNext(nullptr);
    }

    void PipelineState::set_multisample_state()
    {
        multisample_state = vk::PipelineMultisampleStateCreateInfo()
            .setRasterizationSamples(vk::SampleCountFlagBits::e1)
            .setSampleShadingEnable(false)
            .setPSampleMask(nullptr)
            .setAlphaToCoverageEnable(false)
            .setAlphaToOneEnable(false)
            .setPNext(nullptr);
    }

    void PipelineState::set_depth_stencil_state()
    {
        depth_stencil_state = vk::PipelineDepthStencilStateCreateInfo()
            .setDepthTestEnable(true)
            .setDepthWriteEnable(true)
            .setDepthCompareOp(vk::CompareOp::eLess)
            .setDepthBoundsTestEnable(false)
            .setStencilTestEnable(false);
    }

    void PipelineState::set_viewport_state()
    {
        viewport_state = vk::PipelineViewportStateCreateInfo()
            .setViewportCount(1)
            .setPViewports(nullptr)
            .setScissorCount(1)
            .setPScissors(nullptr)
            .setPNext(nullptr);
    }

    void PipelineState::set_dynamic_state()
    {
        dynamic_state = vk::PipelineDynamicStateCreateInfo()
            .setDynamicStateCount(0)
            .setPDynamicStates(nullptr)
            .setPNext(nullptr);
    }

    void PipelineState::set_color_blend_state()
    {
        color_blend_state = vk::PipelineColorBlendStateCreateInfo()
            .setLogicOp(vk::LogicOp::eClear)
            .setLogicOpEnable(false)
            .setAttachmentCount(0)
            .setPAttachments(nullptr)
            .setBlendConstants({ 0.0f, 0.0f, 0.0f, 0.0f })
            .setPNext(nullptr);
    }

    void PipelineState::set_vertex_input_state()
    {
        vertex_input_state = vk::PipelineVertexInputStateCreateInfo()
            .setVertexAttributeDescriptionCount(0)
            .setPVertexAttributeDescriptions(nullptr)
            .setVertexBindingDescriptionCount(0)
            .setPVertexBindingDescriptions(nullptr)
            .setPNext(nullptr);
    }

    vk::PipelineColorBlendAttachmentState
    PipelineState::make_color_blend_attachment_state(vk::ColorComponentFlags color_write_mask,
                                                     vk::Bool32 blend_enable,
                                                     vk::BlendFactor src_color_blend_factor,
                                                     vk::BlendFactor dst_color_blend_factor,
                                                     vk::BlendOp color_blend_op,
                                                     vk::BlendFactor src_alpha_blend_factor,
                                                     vk::BlendFactor dst_alpha_blend_factor,
                                                     vk::BlendOp alpha_blend_op)
    {
        return vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(color_write_mask)
            .setBlendEnable(blend_enable)
            .setSrcColorBlendFactor(src_color_blend_factor)
            .setDstColorBlendFactor(dst_color_blend_factor)
            .setColorBlendOp(color_blend_op)
            .setSrcAlphaBlendFactor(src_alpha_blend_factor)
            .setDstAlphaBlendFactor(dst_alpha_blend_factor)
            .setAlphaBlendOp(alpha_blend_op);
    }
}