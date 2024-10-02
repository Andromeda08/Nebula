#include "Window.hpp"
#include <fmt/format.h>
#include <stdexcept>
#include <vulkan/vk_enum_string_helper.h>

namespace Nebula::wsi
{
    Window::Window(const WindowCreateInfo& create_info)
    : nvk::IWindow(), m_size(create_info.dimensions), m_title(create_info.title)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, false);

        GLFWmonitor* display = create_info.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
        m_window = glfwCreateWindow(static_cast<int>(m_size.width), static_cast<int>(m_size.height),
                                    m_title.c_str(), display, nullptr);

        if (!m_window)
        {
            throw std::runtime_error("Failed to create Window");
        }

        glfwSetKeyCallback(m_window, Window::default_key_handler);
    }

    Window::~Window()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
        }
        glfwTerminate();
    }

    bool Window::will_close()
    {
        bool result = glfwWindowShouldClose(m_window);
        glfwPollEvents();
        return result;
    }

    Size2D Window::size() const
    {
        int width, height;
        glfwGetWindowSize(m_window, &width, &height);
        return {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
    }

    vk::Extent2D Window::framebuffer_size() const
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
    }

    bool Window::create_surface(const vk::Instance& instance, vk::SurfaceKHR* p_surface)
    {
        const VkResult result = glfwCreateWindowSurface(instance, m_window, nullptr, reinterpret_cast<VkSurfaceKHR*>(p_surface));
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failed to create Surface for Window \"{}\": {}", m_title,
                                                 string_VkResult(result)));
        }
        return true;
    }

    void Window::default_key_handler(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }

    std::vector<const char*> Window::get_vulkan_extensions()
    {
        uint32_t extension_count = 0;
        const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);
        return std::vector<const char*>(extensions, extensions + extension_count);
    }
}