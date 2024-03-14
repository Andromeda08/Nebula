#pragma once

#include <string>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.hpp>
#include <nvk/IWindow.hpp>
#include <ncommon/Size2D.hpp>
#include "WindowCreateInfo.hpp"

namespace Nebula::wsi
{
    class Window : public nvk::IWindow
    {
    public:
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        explicit Window(const WindowCreateInfo& create_info);

        ~Window() override;

        bool will_close();

        GLFWwindow* handle() const { return m_window; }

        Size2D size() const;

        vk::Extent2D framebuffer_size() const override;

        bool create_surface(const vk::Instance& instance, vk::SurfaceKHR* p_surface) override;

        std::vector<const char*> get_vulkan_extensions() override;

        HWND get_win32_handle()
        {
            return glfwGetWin32Window(m_window);
        }

    private:
        static void default_key_handler(GLFWwindow* window, int key, int scancode, int action, int mods);

        Size2D          m_size;
        std::string     m_title;
        GLFWwindow*     m_window {nullptr};
    };
}