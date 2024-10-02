#pragma once

#include "Eikon.hpp"
#include "UIItem.hpp"
#include "render/ImGuiRenderer.hpp"

namespace Eikon
{
    struct UserInterfaceParams
    {
        ImGuiRendererParams renderer_params;
    };

    class UserInterface
    {
    public:
        DISABLE_COPY(UserInterface);
        DEF_PRIMARY_CTOR(UserInterface, const UserInterfaceParams& params);

        ~UserInterface() = default;

        void add_item(const std::shared_ptr<UIItem>& item);

        void remove_item(int32_t item_id);

        void render(const vk::CommandBuffer& command_buffer) const;

        void update();

        bool want_capture_mouse() const;

        bool want_capture_keyboard() const;

        bool want_capture_input() const;

    private:
        std::vector<std::shared_ptr<UIItem>> m_ui_items;
        std::shared_ptr<ImGuiRenderer> m_renderer;
    };
}
