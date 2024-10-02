#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <nvk/IWindow.hpp>
#include "Eikon.hpp"

namespace Eikon::wsi
{
    struct WindowCreateInfo
    {
        Size2D      dimensions { 1600, 900 };
        bool        fullscreen {false};
        std::string title {"Nebula"};

        auto& set_size(const Size2D& s)
        {
            dimensions = s;
            return *this;
        }

        auto& set_width(const uint32_t w)
        {
            dimensions.width = w;
            return *this;
        }

        auto& set_height(const uint32_t h)
        {
            dimensions.height = h;
            return *this;
        }

        auto& set_fullscreen(const bool f)
        {
            fullscreen = f;
            return *this;
        }

        auto& set_title(const std::string& t)
        {
            title = t;
            return *this;
        }
    };

    using namespace Nebula;

    class Window final : public nvk::IWindow
    {
    public:
        DISABLE_COPY(Window);

        explicit DEF_PRIMARY_CTOR(Window, const WindowCreateInfo& create_info);

        ~Window() override;

        bool will_close();

        GLFWwindow* handle() const { return m_window; }

        Size2D size() const;

        // nvk::IWindow interface
        vk::Extent2D framebuffer_size() const override;

        bool create_surface(const vk::Instance& instance, vk::SurfaceKHR* p_surface) override;

        std::vector<const char*> get_vulkan_extensions() override;

    private:
        static void default_key_handler(GLFWwindow* window, int key, int scancode, int action, int mods);

        Size2D          m_size;
        std::string     m_title;
        GLFWwindow*     m_window {nullptr};
    };
}