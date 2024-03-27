#pragma once

#include <functional>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "../Image.hpp"
#include "../Device.hpp"
#include "../Utility.hpp"

namespace Nebula::nvk
{
    struct RenderPassCreateInfo
    {
        RenderPassCreateInfo& add_color_attachment(vk::Format            format,
                                                 vk::ImageLayout         final_layout = vk::ImageLayout::eColorAttachmentOptimal,
                                                 vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1,
                                                 vk::ClearColorValue     clear_value  = {0.0f, 0.0f, 0.0f, 1.0f},
                                                 vk::AttachmentLoadOp    load_op      = vk::AttachmentLoadOp::eClear);

        RenderPassCreateInfo& add_attachment(const std::shared_ptr<Image>& image,
                                             vk::ImageLayout final_layout = vk::ImageLayout::eColorAttachmentOptimal,
                                             vk::ClearColorValue     clear_value  = {0.0f, 0.0f, 0.0f, 1.0f},
                                             vk::AttachmentLoadOp load_op = vk::AttachmentLoadOp::eClear);

        RenderPassCreateInfo& set_depth_attachment(vk::Format format,
                                                   vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1,
                                                   vk::ClearDepthStencilValue clear_value = {1.0f, 0});

        RenderPassCreateInfo& set_depth_attachment(const std::shared_ptr<Image>& depth_image,
                                                   vk::ClearDepthStencilValue clear_value = {1.0f, 0});

        RenderPassCreateInfo& set_name(const std::string& value);

        RenderPassCreateInfo& set_render_area(const vk::Rect2D& value);

        std::vector<vk::ClearValue>            clear_values;
        std::vector<vk::AttachmentDescription> attachments;
        std::vector<vk::AttachmentReference>   color_refs;
        vk::AttachmentReference                depth_ref;
        bool                                   has_depth_attachment {false};
        vk::AttachmentReference                resolve_ref;
        bool                                   has_resolve_attachment {false};
        vk::Rect2D                             render_area;
        std::string                            name;
    };

    class RenderPass
    {
    public:
        NVK_DISABLE_COPY(RenderPass);

        RenderPass(const RenderPassCreateInfo& create_info, const std::shared_ptr<Device>& device);

        void set_render_area(const vk::Rect2D& render_area);

        void begin(const vk::CommandBuffer& command_buffer, const vk::Framebuffer& framebuffer);

        void end(const vk::CommandBuffer& command_buffer) const;

        void execute(const vk::CommandBuffer& command_buffer, const vk::Framebuffer& framebuffer,
                     const std::function<void(const vk::CommandBuffer&)>& lambda);

        const vk::RenderPass& render_pass() const { return m_render_pass; }

        struct Builder
        {
            Builder() = default;

            Builder& add_color_attachment(vk::Format format,
                                          vk::ImageLayout final_layout = vk::ImageLayout::eColorAttachmentOptimal,
                                          vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1,
                                          vk::AttachmentLoadOp load_op = vk::AttachmentLoadOp::eClear);

            Builder& set_depth_attachment(vk::Format format,
                                          vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1);

            Builder& set_resolve_attachment(vk::Format format,
                                            vk::ImageLayout final_layout = vk::ImageLayout::ePresentSrcKHR);

            Builder& make_subpass(vk::PipelineBindPoint pipeline_bind_point = vk::PipelineBindPoint::eGraphics);

            Builder& with_name(std::string const& name);

            vk::RenderPass create(const std::shared_ptr<Device>& device);

        private:
            std::vector<vk::AttachmentDescription> _attachments;
            std::vector<vk::AttachmentReference> _color_refs;

            vk::AttachmentReference _depth_ref;
            bool _has_depth_attachment {false};

            vk::AttachmentReference _resolve_ref;
            bool _has_resolve_attachment {false};

            vk::SubpassDescription _subpass = {};
            vk::SubpassDependency  _subpass_dependency = {};

            vk::RenderPass _render_pass;

            std::string _name;
        };

        struct Execute
        {
            Execute() = default;

            Execute& with_render_pass(const vk::RenderPass& render_pass);

            Execute& with_render_area(vk::Rect2D render_area);

            template <unsigned int N>
            Execute& with_clear_values(const std::array<vk::ClearValue, N>& clear_values);

            Execute& with_framebuffer(const vk::Framebuffer& framebuffer);

            void execute(const vk::CommandBuffer& cmd,
                         const std::function<void(const vk::CommandBuffer&)>& fn);

        private:
            vk::RenderPassBeginInfo _begin_info;
        };

    private:
        vk::Rect2D                  m_render_area;
        vk::RenderPass              m_render_pass;
        vk::RenderPassBeginInfo     m_begin_info;
        std::vector<vk::ClearValue> m_clear_values;
        std::shared_ptr<Device>     m_device;
    };

    template<unsigned int N>
    RenderPass::Execute& RenderPass::Execute::with_clear_values(const std::array<vk::ClearValue, N>& clear_values)
    {
        _begin_info.setClearValueCount(clear_values.size());
        _begin_info.setPClearValues(clear_values.data());
        return *this;
    }
}