#pragma once

#include <vulkan/vulkan.hpp>

namespace Nebula::nvk
{
    struct PipelineState
    {
        PipelineState();

        void update();

        vk::PipelineInputAssemblyStateCreateInfo            input_assembly_state;
        vk::PipelineRasterizationStateCreateInfo            rasterization_state;
        vk::PipelineMultisampleStateCreateInfo              multisample_state;
        vk::PipelineDepthStencilStateCreateInfo             depth_stencil_state;
        vk::PipelineViewportStateCreateInfo                 viewport_state;
        vk::PipelineDynamicStateCreateInfo                  dynamic_state;
        vk::PipelineColorBlendStateCreateInfo               color_blend_state;
        vk::PipelineVertexInputStateCreateInfo              vertex_input_state;

        std::vector<vk::VertexInputAttributeDescription>    attribute_descriptions;
        std::vector<vk::VertexInputBindingDescription>      binding_descriptions;
        std::vector<vk::PipelineColorBlendAttachmentState>  attachment_states;
        std::vector<vk::DynamicState>                       dynamic_states { vk::DynamicState::eScissor, vk::DynamicState::eViewport };

        static vk::PipelineColorBlendAttachmentState make_color_blend_attachment_state(
            vk::ColorComponentFlags color_write_mask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
            vk::Bool32              blend_enable = false,
            vk::BlendFactor         src_color_blend_factor = vk::BlendFactor::eOne,
            vk::BlendFactor         dst_color_blend_factor = vk::BlendFactor::eZero,
            vk::BlendOp             color_blend_op = vk::BlendOp::eAdd,
            vk::BlendFactor         src_alpha_blend_factor = vk::BlendFactor::eOne,
            vk::BlendFactor         dst_alpha_blend_factor = vk::BlendFactor::eZero,
            vk::BlendOp             alpha_blend_op = vk::BlendOp::eAdd);

    private:
        void set_input_assembly_state();
        void set_rasterization_state();
        void set_multisample_state();
        void set_depth_stencil_state();
        void set_viewport_state();
        void set_dynamic_state();
        void set_color_blend_state();
        void set_vertex_input_state();
    };
}