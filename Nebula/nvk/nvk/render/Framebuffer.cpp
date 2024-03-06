#include "Framebuffer.hpp"

namespace Nebula::nvk
{
    Framebuffer::Builder& Framebuffer::Builder::add_attachment(const vk::ImageView& image_view)
    {
        _attachments.push_back(image_view);
        return *this;
    }

    Framebuffer::Builder& Framebuffer::Builder::add_attachment_for_index(uint32_t index, const vk::ImageView& image_view)
    {
        _per_fb_attachments.insert({ index, image_view });
        return *this;
    }

    Framebuffer::Builder& Framebuffer::Builder::set_render_pass(const vk::RenderPass& render_pass)
    {
        _render_pass = render_pass;
        return *this;
    }

    Framebuffer::Builder& Framebuffer::Builder::set_size(vk::Extent2D size)
    {
        _size = size;
        return *this;
    }

    Framebuffer::Builder& Framebuffer::Builder::set_count(uint32_t count)
    {
        _count = count;
        return *this;
    }

    Framebuffer::Builder& Framebuffer::Builder::set_name(const std::string& name)
    {
        _name = name;
        return *this;
    }

    std::shared_ptr<Framebuffer> Framebuffer::Builder::create(const std::shared_ptr<Device>& device)
    {
        return _attachments.empty()
               ? std::make_shared<Framebuffer>(_per_fb_attachments, _attachments, _render_pass, _size, _count, device, _name)
               : std::make_shared<Framebuffer>(_attachments, _render_pass, _size, _count, device, _name);
    }

    Framebuffer::Framebuffer(const std::vector<vk::ImageView>& attachments,
                             const vk::RenderPass& render_pass,
                             const vk::Extent2D& size,
                             uint32_t count,
                             const std::shared_ptr<Device>& device,
                             const std::string& name)
    {
        vk::FramebufferCreateInfo create_info;
        create_info.setLayers(1);
        create_info.setAttachmentCount(static_cast<uint32_t>(attachments.size()));
        create_info.setPAttachments(attachments.data());
        create_info.setRenderPass(render_pass);
        create_info.setHeight(size.height);
        create_info.setWidth(size.width);

        m_framebuffers.resize(count);
        for (vk::Framebuffer& framebuffer : m_framebuffers)
        {
            vk::Result result = device->handle().createFramebuffer(&create_info, nullptr, &framebuffer);
            if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error("[Error] Framebuffer creation failed.");
            }
        }

//        for (int32_t i = 0; i < m_framebuffers.size(); i++)
//        {
//            sdvk::util::name_vk_object(std::format("{} {}", name, std::to_string(i)),
//                                       (uint64_t) static_cast<VkFramebuffer>(m_framebuffers[i]),
//                                       vk::ObjectType::eFramebuffer,
//                                       device);
//        }
    }

    Framebuffer::Framebuffer(std::map<uint32_t, vk::ImageView>& per_frame_attachments,
                             const std::vector<vk::ImageView>& attachments,
                             const vk::RenderPass& render_pass,
                             const vk::Extent2D& size,
                             uint32_t count,
                             const std::shared_ptr<Device>& device,
                             const std::string& name)
    {
        vk::FramebufferCreateInfo create_info;
        create_info.setLayers(1);
        create_info.setRenderPass(render_pass);
        create_info.setHeight(size.height);
        create_info.setWidth(size.width);

        m_framebuffers.resize(count);
        for (uint32_t i = 0; i < count; i++)
        {
            std::vector<vk::ImageView> att = attachments;
            att.push_back(per_frame_attachments[i]);

            create_info
                .setPAttachments(att.data())
                .setAttachmentCount(att.size());

            vk::Result result = device->handle().createFramebuffer(&create_info, nullptr, &m_framebuffers[i]);
            if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error("[Error] Framebuffer creation failed.");
            }
        }

//        for (int32_t i = 0; i < m_framebuffers.size(); i++)
//        {
//            sdvk::util::name_vk_object(std::format("{} {}", name, std::to_string(i)),
//                                       (uint64_t) static_cast<VkFramebuffer>(m_framebuffers[i]),
//                                       vk::ObjectType::eFramebuffer,
//                                       device);
//        }
    }

    const vk::Framebuffer& Framebuffer::get(uint32_t index)
    {
        if (index > static_cast<uint32_t>(m_framebuffers.size()))
        {
            throw std::out_of_range("[Error] Index out of range for framebuffer array.");
        }

        return m_framebuffers[index];
    }

    const vk::Framebuffer& Framebuffer::operator[](uint32_t index)
    {
        return get(index);
    }
}