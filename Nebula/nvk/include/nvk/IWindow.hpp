#pragma once

#include <vulkan/vulkan.hpp>

namespace Nebula::nvk
{
    class IWindow
    {
    public:
        virtual vk::Extent2D framebuffer_size() const = 0;
        virtual bool create_surface(const vk::Instance& instance, vk::SurfaceKHR* p_surface) = 0;
        virtual std::vector<const char*> get_vulkan_extensions() = 0;
        virtual ~IWindow() = default;
    };
}