#pragma once

#include "Eikon.hpp"
#include "UserInterface.hpp"
#include "scene/Scene.hpp"

// TODO: move to impl.
#include "wsi/Window.hpp"

namespace Eikon
{
    using namespace Nebula;

    class Application
    {
    public:
        DISABLE_COPY(Application);

        Application()
        {
            const auto window_create_info = wsi::WindowCreateInfo()
                .set_size({ 1920, 1080 })
                .set_fullscreen(false)
                .set_title("Eikon");
            m_window = wsi::Window::createWindow(window_create_info);

            init_rendering_api_context();

            const auto general_queue = m_context->device()->q_general();
            m_context->command_pool()->create_command_ring(2, general_queue);

            const auto swapchain_create_info = nvk::SwapchainCreateInfo()
                .set_context(m_context)
                .set_window(m_window)
                .set_image_count(s_frame_count)
                .set_preferred_format(vk::Format::eR8G8B8A8Unorm)
                .set_preferred_color_space(vk::ColorSpaceKHR::eSrgbNonlinear)
                .set_preferred_present_mode(vk::PresentModeKHR::eMailbox);
            m_swapchain = nvk::Swapchain::create(swapchain_create_info);
        }

    private:
        void init_rendering_api_context()
        {
            std::vector instance_extensions { VK_KHR_SURFACE_EXTENSION_NAME };
            std::vector<const char*> instance_layers {};

            #ifdef _DEBUG
            instance_layers.push_back("VK_LAYER_KHRONOS_validation");
            #endif

            auto context_create_info = nvk::ContextCreateInfo()
                .set_instance_extensions(instance_extensions)
                .set_instance_layers(instance_layers)
                .set_ray_tracing_features(false)
                .set_mesh_shader_features(false)
                .set_window(m_window);

            #ifdef _DEBUG
            context_create_info
                .set_debug_mode(true)
                .set_validation_layers(true);
            #endif

            m_context = std::make_shared<nvk::Context>(context_create_info);
        }

        static constexpr uint32_t s_frame_count = 2;
        static uint32_t s_current_frame;

        std::shared_ptr<wsi::Window>      m_window;

        std::shared_ptr<nvk::Context>     m_context;
        std::shared_ptr<nvk::Swapchain>   m_swapchain;

        std::shared_ptr<UserInterface>    m_user_interface;

        std::shared_ptr<glTFScene>        m_scene;
    };
}