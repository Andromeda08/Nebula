#pragma once

#include <functional>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "../Device.hpp"
#include "../Utility.hpp"

namespace Nebula::nvk
{
    class RenderPass
    {
    public:
        NVK_DISABLE_COPY(RenderPass);

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

    };

template<unsigned int N>
RenderPass::Execute& RenderPass::Execute::with_clear_values(const std::array<vk::ClearValue, N>& clear_values)
{
    _begin_info.setClearValueCount(clear_values.size());
    _begin_info.setPClearValues(clear_values.data());
    return *this;
}
}