#include "VkPlayground.hpp"

namespace Nebula
{
    VkPlayground::VkPlayground()
    : Application(std::make_optional<AppParameters>(false, false, false, true))
    {
    }

    void VkPlayground::render(const vk::CommandBuffer& command_buffer)
    {
        if (m_rg_context->m_render_path)
        {

        }
    }
}