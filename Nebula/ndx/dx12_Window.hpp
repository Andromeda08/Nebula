#pragma once

#include <format>
#include <stdexcept>
#include <string>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <ndx/IWindow.hpp>

namespace Nebula
{
    class DX12_Test_Window : public ndx::IWindow
    {
    public:
        explicit DX12_Test_Window()
        {
            if (!glfwInit())
            {
                throw std::runtime_error("Failed to initialize GLFW");
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, false);

            m_window = glfwCreateWindow(1600, 900, "DirectX 12 Test", nullptr, nullptr);

            if (!m_window)
            {
                throw std::runtime_error("Failed to create Window");
            }

            glfwSetKeyCallback(m_window, DX12_Test_Window::default_key_handler);
        }

        ~DX12_Test_Window() override
        {
            if (m_window)
            {
                glfwDestroyWindow(m_window);
            }
            glfwTerminate();
        }

        bool will_close() const override
        {
            bool result = glfwWindowShouldClose(m_window);
            glfwPollEvents();
            return result;
        }

        Size window_size() const override
        {
            int width, height;
            glfwGetWindowSize(m_window, &width, &height);
            return {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };
        }

        Size framebuffer_size() const override
        {
            int width, height;
            glfwGetFramebufferSize(m_window, &width, &height);
            return {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };
        }

        HWND get_win32_handle() const override
        {
            return glfwGetWin32Window(m_window);
        }

        GLFWwindow* handle() const { return m_window; }

    private:
        static void default_key_handler(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, true);
            }
        }

        std::string     m_title;
        GLFWwindow*     m_window {nullptr};
    };
}