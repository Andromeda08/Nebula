#include "Application.hpp"
#include <iostream>
#include <nvk/render/Framebuffer.hpp>
#include <nvk/render/Pipeline.hpp>
#include <nvk/render/RenderPass.hpp>

namespace Nebula
{
    vk::RenderPass g_render_pass;
    std::shared_ptr<nvk::Pipeline> g_pipeline;
    std::shared_ptr<nvk::Framebuffer> g_framebuffers;
    std::array<vk::ClearValue, 1> g_clear_value {};


    uint32_t Application::s_current_frame = 0;
    Size2D   Application::s_extent = {};

    Application::Application(const std::string& config_json)
    {
        auto wnd_create_info = wsi::WindowCreateInfo()
            .set_size({ m_config.wnd_width, m_config.wnd_height })
            .set_fullscreen(m_config.wnd_fullscreen)
            .set_title(m_config.name);
        m_window = std::make_shared<wsi::Window>(wnd_create_info);

        Application::s_extent = m_window->size();

        init_render_context();

        m_gui = std::make_shared<ngui::GUI>(m_config.gui_font, m_window, m_context, m_swapchain);

        m_active_scene = std::make_shared<ns::Scene>(Size2D { m_swapchain->extent().width, m_swapchain->extent().height },
                                                     "Default Scene",
                                                     m_context->command_pool(),
                                                     m_context->device(),
                                                     true);
        m_scenes.push_back(m_active_scene);

        g_render_pass = nvk::RenderPass::Builder()
            .add_color_attachment(m_swapchain->format(), vk::ImageLayout::ePresentSrcKHR)
            .make_subpass()
            .with_name("Test")
            .create(m_context->device());

        g_clear_value[0].setColor(std::array{ 0.0f, 0.0f, 0.0f, 1.0f });

        nvk::PipelineCreateInfo pipeline_create_info;
        pipeline_create_info
            .set_pipeline_type(nvk::PipelineType::eGraphics)
            .add_shader("fullscreen_quad.vert.spv", vk::ShaderStageFlagBits::eVertex)
            .add_shader("test.frag.spv", vk::ShaderStageFlagBits::eFragment)
            .set_attachment_count(1)
            .set_render_pass(g_render_pass)
            .set_name("test");

        g_pipeline = std::make_shared<nvk::Pipeline>(pipeline_create_info, m_context->device());

        g_framebuffers = nvk::Framebuffer::Builder()
            .add_attachment_for_index(0, m_swapchain->image_view(0))
            .add_attachment_for_index(1, m_swapchain->image_view(1))
            .set_render_pass(g_render_pass)
            .set_size(m_swapchain->extent())
            .set_count(m_swapchain->image_count())
            .set_name("Present Framebuffer")
            .create(m_context->device());
    }

    void Application::run()
    {
        try {
            while (!m_window->will_close())
            {
                loop();
            }
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void Application::loop()
    {
        // Event Handlers here
        if (!m_gui->want_capture_mouse())
        {
            m_active_scene->mouse_handler(*m_window);
        }
        if (!m_gui->want_capture_keyboard())
        {
            m_active_scene->key_handler(*m_window);
        }

        float dt = delta_time();
        m_active_scene->update(dt);

        const uint32_t acquired_frame = m_swapchain->acquire_next_image(s_current_frame);
        const vk::CommandBuffer& command_buffer = m_command_ring->next();
        const vk::CommandBufferBeginInfo begin_info {};
        const vk::Result result = command_buffer.begin(&begin_info);

        m_swapchain->set_viewport_scissor(command_buffer);

        nvk::RenderPass::Execute()
            .with_clear_values<1>(g_clear_value)
            .with_framebuffer(g_framebuffers->get(s_current_frame))
            .with_render_area({ {0, 0}, m_swapchain->extent() })
            .with_render_pass(g_render_pass)
            .execute(command_buffer, [&](const vk::CommandBuffer& cmd) {
                g_pipeline->bind(cmd);
                cmd.draw(3, 1, 0, 0);
            });

        // m_render_graph_ctx->get_render_path()->execute(command_buffer);
        if (m_config.gui_enabled)
        {
            m_gui->render(command_buffer, [&](){});
        }

        command_buffer.end();
        m_swapchain->submit_and_present(s_current_frame, acquired_frame, command_buffer);
        s_current_frame = (s_current_frame + 1) % s_max_frames_in_flight;
        m_context->device()->wait_idle();
    }

    void Application::init_render_context()
    {
        std::vector<const char*> instance_extensions { VK_KHR_SURFACE_EXTENSION_NAME };
        std::vector<const char*> instance_layers { "VK_LAYER_KHRONOS_validation" };
        auto ctx_create_info = nvk::ContextCreateInfo()
            .set_instance_extensions(instance_extensions)
            .set_instance_layers(instance_layers)
            .set_debug_mode(true)
            .set_validation_layers(true)
            .set_ray_tracing_features(true)
            .set_mesh_shader_features(true)
            .set_window(m_window);
        m_context = std::make_shared<nvk::Context>(ctx_create_info);

        auto q_general = m_context->device()->q_general();
        m_command_ring = m_context->command_pool()->create_command_ring(2, q_general);

        auto sc_create_info = nvk::SwapchainCreateInfo()
            .set_context(m_context)
            .set_window(m_window)
            .set_image_count(s_max_frames_in_flight)
            .set_preferred_format(vk::Format::eR8G8B8A8Unorm)
            .set_preferred_color_space(vk::ColorSpaceKHR::eSrgbNonlinear)
            .set_preferred_present_mode(vk::PresentModeKHR::eMailbox);
        m_swapchain = std::make_shared<nvk::Swapchain>(sc_create_info);
    }

    float Application::delta_time()
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> dt = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - m_last_time);
        m_last_time = current_time;
        return dt.count();
    }
}