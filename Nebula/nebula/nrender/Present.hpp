#pragma once

#include <memory>
#include <vector>
#include <nscene/Scene.hpp>
#include <nvk/Barrier.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Command.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Device.hpp>
#include <nvk/Image.hpp>
#include <nvk/Swapchain.hpp>
#include <nvk/render/Framebuffer.hpp>
#include <nvk/render/Pipeline.hpp>
#include <nvk/render/RenderPass.hpp>

namespace Nebula::nrender
{
    class Present
    {
    public:
        Present(const std::shared_ptr<nvk::Device>& device,
                const std::shared_ptr<nvk::Swapchain>& swapchain,
                const std::shared_ptr<nvk::Image>& source)
            : m_device(device), m_swapchain(swapchain), m_source(source)
        {
            using enum nvk::DescriptorType;
            using enum vk::ShaderStageFlagBits;

            auto descriptor_create_info = nvk::DescriptorCreateInfo()
                .add(eCombinedImageSampler, 0, eFragment)
                .set_count(m_swapchain->image_count())
                .set_name("Present");
            m_descriptor = nvk::Descriptor::create(descriptor_create_info, m_device);

            auto render_pass_create_info = nvk::RenderPassCreateInfo()
                .add_color_attachment(swapchain->format(), vk::ImageLayout::ePresentSrcKHR)
                .set_render_area({{0, 0}, swapchain->extent()})
                .set_name("Present");
            m_render_pass = nvk::RenderPass::create(render_pass_create_info, m_device);

            auto framebuffer_create_info = nvk::FramebufferCreateInfo()
                .set_framebuffer_count(swapchain->image_count())
                .add_attachment(swapchain->image_view(0), 0, 0)
                .add_attachment(swapchain->image_view(1), 0, 1)
                .set_render_pass(m_render_pass->render_pass())
                .set_extent(swapchain->extent())
                .set_name("Present");
            m_framebuffers = nvk::Framebuffer::create(framebuffer_create_info, m_device);

            auto pipeline_create_info = nvk::PipelineCreateInfo()
                .set_pipeline_type(nvk::PipelineType::eGraphics)
                .add_descriptor_set_layout(m_descriptor->layout())
                .add_shader("fullscreen_quad.vert.spv", eVertex)
                .add_shader("passthrough.frag.spv", eFragment)
                .set_cull_mode(vk::CullModeFlagBits::eNone)
                .set_sample_count(vk::SampleCountFlagBits::e1)
                .set_render_pass(m_render_pass->render_pass())
                .add_attachment()
                .set_name("Present");
            m_pipeline = nvk::Pipeline::create(pipeline_create_info, m_device);

            for (int32_t i = 0; i < m_descriptor->set_count(); i++)
            {
                vk::DescriptorImageInfo target_info = { m_source->default_sampler(), m_source->image_view(), m_source->state().layout };

                auto write_info = nvk::DescriptorWriteInfo()
                    .set_set_index(i)
                    .add_combined_image_sampler(0, target_info);
                m_descriptor->write(write_info);
            }

            m_size = m_swapchain->extent();
        }

        void render(uint32_t current_frame, const vk::CommandBuffer& command_buffer) const
        {
            m_render_pass->execute(command_buffer, m_framebuffers->get(current_frame), [&](const vk::CommandBuffer& cmd) {
                m_pipeline->bind(command_buffer);
                m_pipeline->bind_descriptor_set(command_buffer, m_descriptor->set(current_frame));
                command_buffer.draw(3, 1, 0, 0);
            });
        }

    private:
        std::shared_ptr<nvk::Image>       m_source;
        vk::Extent2D                      m_size;
        std::shared_ptr<nvk::Pipeline>    m_pipeline;
        std::shared_ptr<nvk::Framebuffer> m_framebuffers;
        std::shared_ptr<nvk::Descriptor>  m_descriptor;
        std::shared_ptr<nvk::RenderPass>  m_render_pass;
        std::shared_ptr<nvk::Device>      m_device;
        std::shared_ptr<nvk::Swapchain>   m_swapchain;
    };
}