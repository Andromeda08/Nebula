#include "Application.hpp"
#include <iostream>

namespace Nebula
{
    uint32_t Application::s_current_frame = 0;

    Application::Application(const std::optional<AppParameters>& params)
    : m_config(AppConfig::load())
    {
        if (params.has_value())
        {
            m_params = params.value();
        }

        auto wnd_create_info = wsi::WindowCreateInfo()
            .set_size({ m_config.wnd_width, m_config.wnd_height })
            .set_fullscreen(m_config.wnd_fullscreen)
            .set_title(m_config.name);
        m_window = std::make_shared<wsi::Window>(wnd_create_info);

        init_render_context();

        m_gui = std::make_shared<ngui::GUI>(m_config.gui_font, m_window, m_context, m_swapchain);

        m_active_scene = std::make_shared<ns::Scene>(Size2D { m_swapchain->extent().width, m_swapchain->extent().height },
                                                     "Default Scene",
                                                     m_context->command_pool(),
                                                     m_context->device(),
                                                     true);
        m_scenes.push_back(m_active_scene);

        // m_rg_context = std::make_shared<nrg::Context>(m_scenes, m_context->device(),
        //                                               m_context->command_pool(), m_swapchain);

        // m_rg_editor = std::make_shared<nrg::GraphEditor>(m_rg_context);
    }

    void Application::run()
    {
        try {
           while (!m_window->will_close()) { loop(); }
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void Application::loop()
    {
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

        update();

        uint32_t acquired_frame = m_swapchain->acquire_next_image(s_current_frame);
        vk::CommandBuffer& command_buffer = m_command_ring->next();
        vk::CommandBufferBeginInfo begin_info {};
        vk::Result result = command_buffer.begin(&begin_info);

        render(command_buffer);

        if (m_config.gui_enabled)
        {
            m_gui->render(command_buffer, [&](){
                render_ui();
            });
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
            .set_ray_tracing_features(m_params.ray_tracing)
            .set_mesh_shader_features(m_params.mesh_shaders)
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