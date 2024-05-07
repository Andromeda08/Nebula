#include "HairRendererApp.hpp"

namespace Nebula
{
    HairRendererApp::HairRendererApp(const std::string& hair_model_path, bool msaa)
    : Application(std::make_optional<AppParameters>({ false, true }))
    {
        m_hair_model = std::make_shared<nhair::HairModel>((hair_model_path.empty() ? "wWavy.hair" : hair_model_path),
                                                          m_context->device(),
                                                          m_context->command_pool());

        m_hair_renderer = std::make_shared<nhair::HairRenderer>(m_context->device(), m_swapchain, msaa);
    }

    void HairRendererApp::update()
    {
        m_hair_model->update();
    }

    void HairRendererApp::render(const vk::CommandBuffer& command_buffer)
    {
        auto camera_data = m_active_scene->active_camera()->uniform_data();

        m_swapchain->set_viewport_scissor(command_buffer);
        m_hair_renderer->render(s_current_frame, *m_hair_model, camera_data, command_buffer);
    }

    void HairRendererApp::render_ui()
    {
        m_hair_renderer->render_ui();
    }
}