#pragma once

#include <memory>

#include <vulkan/vulkan.hpp>

#include <nvk/Context.hpp>
#include <nvk/Swapchain.hpp>
#include <nvk/render/Framebuffer.hpp>
#include <nvk/render/RenderPass.hpp>

#include "Eikon.hpp"
#include "wsi/Window.hpp"

namespace Eikon
{
    using namespace Nebula;

    struct ImGuiRendererParams
    {
        std::string font_file;
        std::shared_ptr<nvk::Context> context;
        std::shared_ptr<nvk::Swapchain> swapchain;
        std::weak_ptr<wsi::Window> window;

        auto& set_font_file(const std::string& value)
        {
            font_file = value;
            return *this;
        }

        auto& set_context(const std::shared_ptr<nvk::Context>& value)
        {
            context = value;
            return *this;
        }

        auto& set_swapchain(const std::shared_ptr<nvk::Swapchain>& value)
        {
            swapchain = value;
            return *this;
        }

        auto& set_window(const std::shared_ptr<wsi::Window>& value)
        {
            window = value;
            return *this;
        }

    };

    class ImGuiRenderer
    {
    public:
        DISABLE_COPY(ImGuiRenderer);
        explicit DEF_PRIMARY_CTOR(ImGuiRenderer, const ImGuiRendererParams& params);

        ~ImGuiRenderer() = default;

        void render(const vk::CommandBuffer& command_buffer,
                    const std::vector<std::shared_ptr<struct UIItem>>& ui_items);

        void set_mode(bool ui_only);

    private:
        void init_resources();
        void init_imgui(const ImGuiRendererParams& params) const;

        vk::DescriptorPool m_descriptor_pool;
        vk::PipelineCache m_pipeline_cache;

        std::shared_ptr<nvk::RenderPass> m_active_render_pass;
        std::shared_ptr<nvk::RenderPass> m_rp_default;
        std::shared_ptr<nvk::RenderPass> m_rp_ui_only;

        std::shared_ptr<nvk::Framebuffer> m_framebuffers;
        std::unique_ptr<RingWrapper<vk::Framebuffer>> m_framebuffer_ring;

        std::shared_ptr<nvk::Context> m_context;
        std::shared_ptr<nvk::Swapchain> m_swapchain;
    };
}
