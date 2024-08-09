#include "Application.hpp"
#include <iostream>
#include <nscene/DefaultScene.hpp>

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

        if (m_params.default_scene)
        {
            m_active_scene = std::make_shared<ns::DefaultScene>(glm::ivec2(m_swapchain->extent().width, m_swapchain->extent().height),
                                                                m_context->command_pool(), m_context->device());
            m_active_scene->init();
            m_scenes.push_back(m_active_scene);
        }

        if (m_params.render_graph)
        {
            m_rg_context = std::make_shared<nrg::Context>(m_scenes, m_context->device(), m_context->command_pool(), m_swapchain, s_current_frame);
            m_rg_editor  = std::make_shared<nrg::GraphEditor>(m_rg_context);
        }
    }

    void Application::run()
    {
        try {
           while (!m_window->will_close())
           {
               loop();
               if (m_shutdown_requested) break;
           }
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void Application::loop()
    {
        m_has_rendered = false;

        if (!m_gui->want_capture_mouse())
        {
            m_active_scene->mouse_handler(*m_window);
        }
        if (!m_gui->want_capture_keyboard())
        {
            m_active_scene->key_handler(*m_window);
        }

        float dt = delta_time();
        m_active_scene->update(dt, s_current_frame);

        update();

        uint32_t acquired_frame = m_swapchain->acquire_next_image(s_current_frame);
        vk::CommandBuffer& command_buffer = m_command_ring->next();
        vk::CommandBufferBeginInfo begin_info {};
        vk::Result result = command_buffer.begin(&begin_info);

        render(command_buffer);

        if (m_config.gui_enabled)
        {
            m_gui->render(command_buffer, [&](){
                if (m_params.render_graph) {
                    m_rg_editor->render();
                }
                render_ui();
            }, !m_has_rendered);
        }

        command_buffer.end();
        m_swapchain->submit_and_present(s_current_frame, acquired_frame, command_buffer);
        s_current_frame = (s_current_frame + 1) % s_max_frames_in_flight;
        m_context->device()->wait_idle();
        m_has_rendered = false;
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

    void Application::request_shutdown()
    {
        m_shutdown_requested = true;
    }

    void Application::set_has_rendered()
    {
        m_has_rendered = true;
    }
}