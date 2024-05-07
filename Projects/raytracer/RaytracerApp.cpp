#include "RaytracerApp.hpp"

namespace Nebula
{
    RaytracerApp::RaytracerApp()
    : Application(std::make_optional<AppParameters>({ true, false }))
    {
        m_raytracer   = std::make_shared<nrender::Raytracer>(m_context->device(), m_context->command_pool(), m_swapchain, m_active_scene);
        m_light_debug = std::make_shared<nrender::DebugRender>(m_context->device(), m_swapchain, m_active_scene, m_raytracer->target());
        m_present     = std::make_shared<nrender::Present>(m_context->device(), m_swapchain, m_light_debug->target());
    }

    void RaytracerApp::update()
    {

    }

    void RaytracerApp::render(const vk::CommandBuffer& command_buffer)
    {
        nvk::ImageBarrier(m_raytracer->target(), m_raytracer->target()->state().layout, vk::ImageLayout::eGeneral).apply(command_buffer);
        m_raytracer->render(s_current_frame, command_buffer);

        m_swapchain->set_viewport_scissor(command_buffer);

        m_light_debug->render(s_current_frame, command_buffer, m_raytracer->rt_light());

        if (m_raytracer->rt_light().light_type == 0)
        {
            nvk::ImageBarrier(m_raytracer->target(), vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eGeneral).apply(command_buffer);
        }

        m_present->render(s_current_frame, command_buffer);
    }

    void RaytracerApp::render_ui()
    {
//        ImGui::Begin("Options");
//        {
//            ImGui::Checkbox("Render Lights", &m_render_debug_lights);
//        }
//        ImGui::End();

        m_raytracer->render_ui();
    }
}