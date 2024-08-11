#include "Present.hpp"
#include "nrg/common/ResourceTraits.hpp"
#include "nrg/resource/Resources.hpp"

namespace Nebula::nrg
{
    nrg_def_resource_requirements(Present, ({
        std::make_shared<ImageRequirement>(s_input, ResourceUsage::eInput, ResourceType::eImage, vk::ImageLayout::eShaderReadOnlyOptimal, vk::Format::eR32G32B32A32Sfloat),
    }))

    void Present::initialize()
    {
        using SSFB = vk::ShaderStageFlagBits;

        auto descriptor_create_info = nvk::DescriptorCreateInfo()
            .add(nvk::DescriptorType::eCombinedImageSampler, 0, SSFB::eFragment)
            .set_count(2)
            .set_name("Present");
        m_descriptor = std::make_shared<nvk::Descriptor>(descriptor_create_info, m_device);

        auto render_pass_create_info = nvk::RenderPassCreateInfo()
            .add_color_attachment(m_swapchain->format(), vk::ImageLayout::ePresentSrcKHR)
            .set_name("Present RenderPass")
            .set_render_area({{0,0}, m_swapchain->extent()});
        m_render_pass = std::make_shared<nvk::RenderPass>(render_pass_create_info, m_device);

        auto framebuffer_create_info = nvk::FramebufferCreateInfo()
            .set_framebuffer_count(m_swapchain->image_count())
            .set_render_pass(m_render_pass->render_pass())
            .set_extent(m_swapchain->extent())
            .set_name("Present")
            .add_attachment(m_swapchain->image_view(0), 0, 0)
            .add_attachment(m_swapchain->image_view(1), 0, 1);
        m_framebuffers = std::make_shared<nvk::Framebuffer>(framebuffer_create_info, m_device);

        auto pipeline_create_info = nvk::PipelineCreateInfo()
            .set_pipeline_type(nvk::PipelineType::eGraphics)
            .add_descriptor_set_layout(m_descriptor->layout())
            .add_shader("fullscreen_quad.vert.spv", vk::ShaderStageFlagBits::eVertex)
            .add_shader("nrg_present.frag.spv", vk::ShaderStageFlagBits::eFragment)
            .set_attachment_count(1)
            .set_sample_count(vk::SampleCountFlagBits::e1)
            .set_render_pass(m_render_pass->render_pass())
            .set_cull_mode(vk::CullModeFlagBits::eNone)
            .set_name("Present");
        m_pipeline = std::make_shared<nvk::Pipeline>(pipeline_create_info, m_device);

        const auto& input = get_resource<ImageResource>(s_input).get_image();
        vk::DescriptorImageInfo input_info = { input->default_sampler(), input->image_view(), vk::ImageLayout::eShaderReadOnlyOptimal };

        for (int32_t i = 0; i < m_context->m_frames; i++)
        {
            auto write_info = nvk::DescriptorWriteInfo()
                .set_set_index(i)
                .add_combined_image_sampler(0, input_info);
            m_descriptor->write(write_info);
        }
    }

    void Present::execute(const vk::CommandBuffer& command_buffer)
    {
        m_render_pass->execute(command_buffer, m_framebuffers->get(m_current_frame), [&](const vk::CommandBuffer& cmd) {
            m_pipeline->bind(cmd);
            cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline->layout(), 0, 1, &m_descriptor->set(m_current_frame), 0, nullptr);
            cmd.draw(3, 1, 0, 0);
        });
    }

    void Present::update()
    {
    }
}