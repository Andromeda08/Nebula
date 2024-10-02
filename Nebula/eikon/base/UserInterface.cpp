#include "UserInterface.hpp"

#include <imgui.h>

namespace Eikon
{
    UserInterface::UserInterface(const UserInterfaceParams& params)
    {
        m_renderer = ImGuiRenderer::createImGuiRenderer(params.renderer_params);
    }

    std::shared_ptr<UserInterface> UserInterface::createUserInterface(const UserInterfaceParams& params)
    {
        return std::make_shared<UserInterface>(params);
    }

    void UserInterface::add_item(const std::shared_ptr<UIItem>& item)
    {
        m_ui_items.push_back(item);
    }

    void UserInterface::remove_item(const int32_t item_id)
    {
        if (const auto it = std::ranges::find_if(m_ui_items, [item_id](const auto& item){ return item->id() == item_id; });
            it != std::end(m_ui_items))
        {
            m_ui_items.erase(it);
        }
    }

    void UserInterface::render(const vk::CommandBuffer& command_buffer) const
    {
        m_renderer->render(command_buffer, m_ui_items);
    }

    void UserInterface::update()
    {
        for (auto& ui_item : m_ui_items)
        {
            ui_item->update();
        }
    }

    bool UserInterface::want_capture_mouse() const
    {
        const ImGuiIO& io = ImGui::GetIO();
        return io.WantCaptureMouse;
    }

    bool UserInterface::want_capture_keyboard() const
    {
        const ImGuiIO& io = ImGui::GetIO();
        return io.WantCaptureKeyboard;
    }

    bool UserInterface::want_capture_input() const
    {
        return want_capture_mouse() || want_capture_keyboard();
    }
}
