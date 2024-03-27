#pragma once

#include <array>
#include <functional>
#include <memory>
#include <vulkan/vulkan.hpp>
#include <nvk/Context.hpp>
#include <nvk/Swapchain.hpp>
#include <nvk/render/Framebuffer.hpp>
#include <nvk/render/RenderPass.hpp>
#include <wsi/Window.hpp>

namespace Nebula::ngui
{
    class GUI
    {
    public:
        GUI(const GUI&) = delete;
        GUI& operator=(const GUI&) = delete;

        GUI(const std::string& font_path,
            const std::shared_ptr<wsi::Window>& window,
            const std::shared_ptr<nvk::Context>& context,
            const std::shared_ptr<nvk::Swapchain>& swapchain);

        ~GUI() = default;

        void render(const vk::CommandBuffer& command_buffer,
                    const std::function<void()>& lambda);

        bool want_capture_mouse() const;

        bool want_capture_keyboard() const;

    private:
        void init_imgui(const std::string& font_path);

        float get_ui_scale() const;

        float get_font_size() const;

        vk::DescriptorPool                m_descriptor_pool;
        vk::PipelineCache                 m_pipeline_cache;
        std::shared_ptr<nvk::RenderPass>  m_render_pass;
        std::shared_ptr<nvk::Framebuffer> m_framebuffers;
        uint32_t                          m_next_framebuffer {0};
        std::shared_ptr<wsi::Window>      m_window;
        std::shared_ptr<nvk::Context>     m_context;
        std::shared_ptr<nvk::Swapchain>   m_swapchain;
    };
}