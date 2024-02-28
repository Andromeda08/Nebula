#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Context.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Image.hpp>
#include <nvk/Swapchain.hpp>
#include <nvk/render/Framebuffer.hpp>
#include <nvk/render/Pipeline.hpp>
#include <nvk/render/RenderPass.hpp>
#include <nvk/render/PipelineCreateInfo.hpp>
#include <wsi/Window.hpp>
#include <wsi/WindowCreateInfo.hpp>

using namespace Nebula;

int main()
{
    try {
        std::vector<const char*> instance_extensions{ VK_KHR_SURFACE_EXTENSION_NAME };
        std::vector<const char*> instance_layers{ "VK_LAYER_KHRONOS_validation" };

        wsi::WindowCreateInfo wnd_create_info{};
        wnd_create_info
            .set_size({ 1600, 900 })
            .set_fullscreen(false)
            .set_title("Nebula test Window");

        auto window = std::make_shared<wsi::Window>(wnd_create_info);

        nvk::ContextCreateInfo ctx_create_info{};
        ctx_create_info
            .set_instance_extensions(instance_extensions)
            .set_instance_layers(instance_layers)
            .set_debug_mode(true)
            .set_validation_layers(true)
            .set_ray_tracing_features(true)
            .set_mesh_shader_features(true)
            .set_window(window);

        auto context = std::make_shared<nvk::Context>(ctx_create_info);

        auto q_general = context->device()->q_general();
        auto command_ring = context->command_pool()->create_command_ring<2>(q_general);

        nvk::SwapchainCreateInfo sc_create_info{};
        sc_create_info
            .set_context(context)
            .set_window(window)
            .set_image_count(2)
            .set_preferred_format(vk::Format::eR8G8B8A8Unorm)
            .set_preferred_color_space(vk::ColorSpaceKHR::eSrgbNonlinear)
            .set_preferred_present_mode(vk::PresentModeKHR::eMailbox);

        auto swapchain = std::make_shared<nvk::Swapchain>(sc_create_info);

        nvk::ImageCreateInfo img_create_info{};
        img_create_info.set_name("test");

        auto image = std::make_shared<nvk::Image>(img_create_info, context->device());

        nvk::BufferCreateInfo buf_create_info{};
        buf_create_info
            .set_buffer_type(nvk::BufferType::eVertex)
            .set_name("test")
            .set_size(128);

        auto buffers = std::make_shared<nvk::Buffer>(buf_create_info, context->device());

        nvk::DescriptorCreateInfo descriptor_create_info;
        descriptor_create_info
            .add(Nebula::nvk::DescriptorType::eUniformBuffer, 0, vk::ShaderStageFlagBits::eVertex)
            .set_count(2)
            .set_name("test")
            .enable_ring_mode();

        auto descriptor = std::make_shared<nvk::Descriptor>(descriptor_create_info, context->device());
        auto& descriptor_ring = descriptor->get_ring();

        auto render_pass = nvk::RenderPass::Builder()
            .add_color_attachment(swapchain->format(), vk::ImageLayout::ePresentSrcKHR)
            .make_subpass()
            .with_name("Test")
            .create(context->device());

        std::array<vk::ClearValue, 1> clear_value{};
        clear_value[0].setColor(std::array{ 0.0f, 0.0f, 0.0f, 1.0f });

        nvk::PipelineCreateInfo pipeline_create_info;
        pipeline_create_info
            .set_pipeline_type(nvk::PipelineType::eGraphics)
            .add_shader("fullscreen_quad.vert.spv", vk::ShaderStageFlagBits::eVertex)
            .add_shader("test.frag.spv", vk::ShaderStageFlagBits::eFragment)
            .set_attachment_count(1)
            .set_render_pass(render_pass)
            .set_name("test");

        auto pipeline = std::make_shared<nvk::Pipeline>(pipeline_create_info, context->device());

        auto framebuffers = nvk::Framebuffer::Builder()
            .add_attachment_for_index(0, swapchain->image_view(0))
            .add_attachment_for_index(1, swapchain->image_view(1))
            .set_render_pass(render_pass)
            .set_size(swapchain->extent())
            .set_count(swapchain->image_count())
            .set_name("Present Framebuffer")
            .create(context->device());

        uint32_t current_frame = 0;
        while (!window->will_close())
        {
            const auto acquired_frame = swapchain->acquire_next_image(current_frame);

            const auto command_buffer = command_ring->next();

            vk::CommandBufferBeginInfo begin_info;
            const auto result = command_buffer.begin(&begin_info);

            const auto vp = swapchain->get_viewport();
            const auto sc = swapchain->get_scissor();
            command_buffer.setViewport(0, 1, &vp);
            command_buffer.setScissor(0, 1, &sc);

            nvk::RenderPass::Execute()
                .with_clear_values<1>(clear_value)
                .with_framebuffer(framebuffers->get(current_frame))
                .with_render_area({ {0, 0}, swapchain->extent() })
                .with_render_pass(render_pass)
                .execute(command_buffer, [&](const vk::CommandBuffer& cmd) {
                    pipeline->bind(cmd);
                    cmd.draw(3, 1, 0, 0);
                });

            command_buffer.end();

            swapchain->submit_and_present(current_frame, acquired_frame, command_buffer);
            current_frame = (current_frame + 1) % swapchain->image_count();
            context->device()->handle().waitIdle();
        }
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}