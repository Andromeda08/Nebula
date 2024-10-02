#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "Context.hpp"
#include "Device.hpp"
#include "IWindow.hpp"
#include "Utility.hpp"

namespace Nebula::nvk
{
    struct SwapchainCreateInfo
    {
        std::shared_ptr<IWindow>    window;
        std::shared_ptr<Context>    context;
        uint32_t                    image_count {2};
        vk::Format                  pref_format {vk::Format::eB8G8R8A8Unorm};
        vk::ColorSpaceKHR           pref_color_space {vk::ColorSpaceKHR::eSrgbNonlinear};
        vk::PresentModeKHR          pref_present_mode {vk::PresentModeKHR::eMailbox};

        inline SwapchainCreateInfo& set_context(const std::shared_ptr<Context>& value)
        {
            context = value;
            return *this;
        }

        inline SwapchainCreateInfo& set_window(const std::shared_ptr<IWindow>& value)
        {
            window = value;
            return *this;
        }

        inline SwapchainCreateInfo& set_image_count(uint32_t value)
        {
            image_count = value;
            return *this;
        }

        inline SwapchainCreateInfo& set_preferred_format(vk::Format value)
        {
            pref_format = value;
            return *this;
        }

        inline SwapchainCreateInfo& set_preferred_color_space(vk::ColorSpaceKHR value)
        {
            pref_color_space = value;
            return *this;
        }

        inline SwapchainCreateInfo& set_preferred_present_mode(vk::PresentModeKHR value)
        {
            pref_present_mode = value;
            return *this;
        }
    };

    class Swapchain
    {
    public:
        NVK_DISABLE_COPY(Swapchain)

        explicit Swapchain(const SwapchainCreateInfo& create_info);

        static std::shared_ptr<Swapchain> create(const SwapchainCreateInfo& create_info);

        ~Swapchain();

        uint32_t acquire_next_image(uint32_t current_frame) const;

        void submit_and_present(uint32_t current_frame, uint32_t acquired_image, const vk::CommandBuffer& command_buffer) const;

        void set_viewport_scissor(const vk::CommandBuffer& command_buffer) const;

        const vk::Rect2D& get_scissor() const;

        const vk::Viewport& get_viewport() const;

        float aspect_ratio() const;

        const vk::Image& image(uint32_t i) const;

        const vk::ImageView& image_view(uint32_t i) const;

        const vk::Format& format() const { return m_format; }

        const vk::Extent2D& extent() const { return m_size; }

        const uint32_t image_count() const { return m_image_count; }

        void destroy();

    private:
        void set_swapchain_parameters(const SwapchainCreateInfo& create_info);

        void create_swapchain();

        void acquire_images();

        void create_image_views();

        void create_sync_objects();

        void make_scissor_and_viewport();

        uint32_t                    m_image_count {2};
        vk::Extent2D                m_size {};
        vk::Format                  m_format {vk::Format::eB8G8R8A8Unorm};
        vk::ColorSpaceKHR           m_color_space {vk::ColorSpaceKHR::eSrgbNonlinear};
        vk::PresentModeKHR          m_present_mode {vk::PresentModeKHR::eMailbox};

        vk::SurfaceTransformFlagBitsKHR m_pre_transform;

        vk::SwapchainKHR            m_swapchain;

        vk::Rect2D                  m_cached_scissor;
        vk::Viewport                m_cached_viewport;

        std::vector<vk::Image>      m_images;
        std::vector<vk::ImageView>  m_image_views;

        std::vector<vk::Semaphore>  m_image_ready;
        std::vector<vk::Semaphore>  m_rendering_finished;
        std::vector<vk::Fence>      m_frame_in_flight;

        std::shared_ptr<IWindow>    m_window;
        std::shared_ptr<Device>     m_device;
        const vk::SurfaceKHR&       m_surface;
    };
}