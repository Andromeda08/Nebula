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
                                             vk::ImageLayout              final_layout = vk::ImageLayout::eColorAttachmentOptimal,
                                             vk::ClearColorValue          clear_value  = {0.0f, 0.0f, 0.0f, 1.0f},
                                             vk::AttachmentLoadOp         load_op = vk::AttachmentLoadOp::eClear);

        RenderPassCreateInfo& set_depth_attachment(vk::Format                 format,
                                                   vk::SampleCountFlagBits    sample_count = vk::SampleCountFlagBits::e1,
                                                   vk::ClearDepthStencilValue clear_value = {1.0f, 0});

        RenderPassCreateInfo& set_depth_attachment(const std::shared_ptr<Image>& depth_image,
                                                   vk::ClearDepthStencilValue    clear_value = {1.0f, 0});

        RenderPassCreateInfo& set_resolve_attachment(vk::Format              format,
                                                     vk::ImageLayout         final_layout = vk::ImageLayout::eColorAttachmentOptimal,
                                                     vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1,
                                                     vk::ClearColorValue     clear_value = {0.0f, 0.0f, 0.0f, 1.0f});

        RenderPassCreateInfo& set_resolve_attachment(const std::shared_ptr<Image>& resolve_image,
                                                     vk::ImageLayout               final_layout = vk::ImageLayout::eColorAttachmentOptimal,
                                                     vk::ClearColorValue           clear_value = {0.0f, 0.0f, 0.0f, 1.0f});

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

    private:
        vk::Rect2D                  m_render_area;
        vk::RenderPass              m_render_pass;
        vk::RenderPassBeginInfo     m_begin_info;
        std::vector<vk::ClearValue> m_clear_values;
        std::shared_ptr<Device>     m_device;
    };
}