#include "Application.hpp"
#include <iostream>

namespace Nebula
{
    uint32_t Application::s_current_frame = 0;
    Size2D   Application::s_extent = {};

    Application::Application(std::optional<std::string> hair_file)
    : m_config(AppConfig::load())
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

        m_rg_context = std::make_shared<nrg::Context>(m_scenes, m_context->device(),
                                                      m_context->command_pool(), m_swapchain);

        m_rg_editor = std::make_shared<nrg::GraphEditor>(m_rg_context);

        m_hair_model = std::make_shared<nhair::HairModel>((hair_file.has_value() ? hair_file.value() : "wWavy.hair"), m_context->device(), m_context->command_pool());

        m_hair_renderer = std::make_shared<nhair::HairRenderer>(m_context->device(), m_swapchain, true);
        m_ray_tracer    = std::make_shared<nrender::Raytracer>(m_context->device(), m_context->command_pool(), m_swapchain, m_active_scene);
        m_light_debug   = std::make_shared<nrender::DebugRender>(m_context->device(), m_swapchain, m_active_scene, m_ray_tracer->target());
        m_present       = std::make_shared<nrender::Present>(m_context->device(), m_swapchain, m_light_debug->target());

        m_sbuffer       = std::make_shared<nrender::SBuffer>(m_swapchain->extent(),
                                                             m_context->device(),
                                                             m_context->command_pool(),
                                                             m_swapchain,
                                                             m_active_scene,
                                                             m_hair_model);
    }

    void Application::run()
    {
        try {
           while (!m_window->will_close()) { loop(); }
           // loop();
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

        auto camera_data = m_active_scene->active_camera()->uniform_data();

        uint32_t acquired_frame = m_swapchain->acquire_next_image(s_current_frame);
        vk::CommandBuffer& command_buffer = m_command_ring->next();
        vk::CommandBufferBeginInfo begin_info {};
        vk::Result result = command_buffer.begin(&begin_info);

        // m_hair_model->update(command_buffer);
        // m_hair_renderer->render(s_current_frame, *m_hair_model, camera_data, command_buffer);

        if (m_rg_context->m_render_path)
        {
            m_rg_context->m_render_path->execute(command_buffer);
        }

        nvk::ImageBarrier(m_ray_tracer->target(), m_ray_tracer->target()->state().layout, vk::ImageLayout::eGeneral).apply(command_buffer);
        m_ray_tracer->render(s_current_frame, command_buffer);

        m_swapchain->set_viewport_scissor(command_buffer);
        m_light_debug->render(s_current_frame, command_buffer, m_ray_tracer->rt_light());

        if (m_ray_tracer->rt_light().light_type == 0)
            nvk::ImageBarrier(m_ray_tracer->target(), vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eGeneral).apply(command_buffer);

        m_present->render(s_current_frame, command_buffer);

        // m_sbuffer->init(command_buffer);
        // m_sbuffer->execute(s_current_frame, command_buffer);

        if (m_config.gui_enabled)
        {
            m_gui->render(command_buffer, [&](){
                ImGui::Begin("Hair Parameters");
                ImGui::SliderFloat3("Diffuse", m_hair_renderer->hair_diffuse.data(), 0.0f, 1.0f);
                ImGui::SliderFloat3("Specular", m_hair_renderer->hair_specular.data(), 0.0f, 1.0f);
                ImGui::End();

                m_rg_editor->render();
                m_ray_tracer->render_ui();
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