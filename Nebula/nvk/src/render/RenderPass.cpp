#include "render/RenderPass.hpp"
#include <nlog/nlog.hpp>

#ifdef NVK_VERBOSE_EXTRA
#include <iostream>
#endif

namespace Nebula::nvk
{
    RenderPassCreateInfo&
    RenderPassCreateInfo::add_color_attachment(vk::Format format, vk::ImageLayout final_layout, vk::SampleCountFlagBits sample_count,
                                              vk::ClearColorValue clear_value, vk::AttachmentLoadOp load_op)
    {
        auto ad = vk::AttachmentDescription()
            .setFormat(format)
            .setSamples(sample_count)
            .setLoadOp(load_op)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(final_layout);
        attachments.push_back(ad);

        auto ref = vk::AttachmentReference()
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setAttachment(attachments.size() - 1);
        color_refs.push_back(ref);

        auto cv = vk::ClearValue(clear_value);
        clear_values.push_back(cv);

        return *this;
    }

    RenderPassCreateInfo&
    RenderPassCreateInfo::add_attachment(const std::shared_ptr<Image>& image, vk::ImageLayout final_layout,
                                         vk::ClearColorValue clear_value, vk::AttachmentLoadOp load_op)
    {
        auto ad = vk::AttachmentDescription()
            .setFormat(image->properties().format)
            .setSamples(image->properties().sample_count)
            .setLoadOp(load_op)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(final_layout);
        attachments.push_back(ad);

        auto ref = vk::AttachmentReference()
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setAttachment(attachments.size() - 1);
        color_refs.push_back(ref);

        auto cv = vk::ClearValue(clear_value);
        clear_values.push_back(cv);

        return *this;
    }

    RenderPassCreateInfo&
    RenderPassCreateInfo::set_depth_attachment(vk::Format format, vk::SampleCountFlagBits sample_count,
                                               vk::ClearDepthStencilValue clear_value)
    {
        auto ad = vk::AttachmentDescription()
            .setFormat(format)
            .setSamples(sample_count)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
        attachments.push_back(ad);

        has_depth_attachment = true;
        depth_ref.setAttachment(attachments.size() - 1);
        depth_ref.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        auto cv = vk::ClearValue();
        cv.depthStencil = clear_value;
        clear_values.push_back(cv);

        return *this;
    }

    RenderPassCreateInfo&
    RenderPassCreateInfo::set_depth_attachment(const std::shared_ptr<Image>& depth_image, vk::ClearDepthStencilValue clear_value)
    {
        auto ad = vk::AttachmentDescription()
            .setFormat(depth_image->properties().format)
            .setSamples(depth_image->properties().sample_count)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
        attachments.push_back(ad);

        has_depth_attachment = true;
        depth_ref.setAttachment(attachments.size() - 1);
        depth_ref.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        auto cv = vk::ClearValue();
        cv.depthStencil = clear_value;
        clear_values.push_back(cv);

        return *this;
    }

    RenderPassCreateInfo& RenderPassCreateInfo::set_name(const std::string& value)
    {
        name = value;
        return *this;
    }

    RenderPassCreateInfo& RenderPassCreateInfo::set_render_area(const vk::Rect2D& value)
    {
        render_area = value;
        return *this;
    }

    RenderPassCreateInfo&
    RenderPassCreateInfo::set_resolve_attachment(vk::Format format, vk::ImageLayout final_layout,
                                                 vk::SampleCountFlagBits sample_count, vk::ClearColorValue clear_value)
    {
        auto ad = vk::AttachmentDescription()
            .setFormat(format)
            .setSamples(sample_count)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(final_layout);
        attachments.push_back(ad);

        has_resolve_attachment = true;
        resolve_ref.setAttachment(attachments.size() - 1);
        resolve_ref.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        auto cv = vk::ClearValue();
        cv.color = clear_value;
        clear_values.push_back(cv);

        return *this;
    }

    RenderPassCreateInfo&
    RenderPassCreateInfo::set_resolve_attachment(const std::shared_ptr<Image>& resolve_image, vk::ImageLayout final_layout,
                                                 vk::ClearColorValue clear_value)
    {
        auto ad = vk::AttachmentDescription()
            .setFormat(resolve_image->properties().format)
            .setSamples(resolve_image->properties().sample_count)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(final_layout);
        attachments.push_back(ad);

        has_resolve_attachment = true;
        resolve_ref.setAttachment(attachments.size() - 1);
        resolve_ref.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        auto cv = vk::ClearValue();
        cv.color = clear_value;
        clear_values.push_back(cv);

        return *this;
    }

    RenderPass::RenderPass(RenderPassCreateInfo& create_info, const std::shared_ptr<Device>& device)
    : m_device(device), m_render_area(create_info.render_area), m_clear_values(create_info.clear_values)
    {
        auto subpass = vk::SubpassDescription()
            .setColorAttachmentCount(static_cast<uint32_t>(create_info.color_refs.size()))
            .setInputAttachmentCount(0)
            .setPInputAttachments(nullptr)
            .setPResolveAttachments(create_info.has_resolve_attachment ? &create_info.resolve_ref : nullptr)
            .setPColorAttachments(create_info.color_refs.data())
            .setPDepthStencilAttachment(create_info.has_depth_attachment ? &create_info.depth_ref : nullptr)
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

        auto subpass_dependency = vk::SubpassDependency()
            .setSrcSubpass(VK_SUBPASS_EXTERNAL)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
            .setSrcAccessMask({})
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);


        auto rp_create_info = vk::RenderPassCreateInfo()
            .setAttachmentCount(create_info.attachments.size())
            .setPAttachments(create_info.attachments.data())
            .setSubpassCount(1)
            .setPSubpasses(&subpass)
            .setDependencyCount(1)
            .setPDependencies(&subpass_dependency);

        if (const vk::Result result = m_device->handle().createRenderPass(&rp_create_info, nullptr, &m_render_pass);
            result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to create vk::RenderPass {}: {}", create_info.name, to_string(result));
        }

        m_device->name_object(m_render_pass, create_info.name, vk::ObjectType::eRenderPass);

        m_begin_info = vk::RenderPassBeginInfo()
            .setRenderArea(m_render_area)
            .setRenderPass(m_render_pass)
            .setClearValueCount(m_clear_values.size())
            .setPClearValues(m_clear_values.data());
    }

    void RenderPass::set_render_area(const vk::Rect2D& render_area)
    {
        m_render_area = render_area;
        m_begin_info.setRenderArea(m_render_area);
    }

    void RenderPass::begin(const vk::CommandBuffer& command_buffer, const vk::Framebuffer& framebuffer)
    {
        m_begin_info.setFramebuffer(framebuffer);
        command_buffer.beginRenderPass(&m_begin_info, vk::SubpassContents::eInline);
    }

    void RenderPass::end(const vk::CommandBuffer& command_buffer) const
    {
        command_buffer.endRenderPass();
    }

    void RenderPass::execute(const vk::CommandBuffer& command_buffer, const vk::Framebuffer& framebuffer,
                             const std::function<void(const vk::CommandBuffer&)>& lambda)
    {
        begin(command_buffer, framebuffer);
        lambda(command_buffer);
        end(command_buffer);
    }


}