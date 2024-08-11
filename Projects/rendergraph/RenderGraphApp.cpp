#include "RenderGraphApp.hpp"

namespace Nebula
{
    RenderGraphApp::RenderGraphApp()
    : Application(std::make_optional<AppParameters>({ false, false, true, true }))
    {
    }

    void RenderGraphApp::render(const vk::CommandBuffer& command_buffer)
    {
        if (m_rg_context->m_render_path)
        {
            m_swapchain->set_viewport_scissor(command_buffer);
            m_rg_context->m_render_path->execute(command_buffer);
            set_has_rendered();
            // request_shutdown();
        }
    }
}