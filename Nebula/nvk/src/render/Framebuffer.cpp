#include "render/Framebuffer.hpp"
#include <nlog/nlog.hpp>

#include <iostream>
#include <format>
#include <sstream>

namespace Nebula::nvk
{
    FramebufferCreateInfo&
    FramebufferCreateInfo::add_attachment(const vk::ImageView& image_view, std::optional<uint32_t> attachment_idx,
                                          std::optional<uint32_t> fb_idx)
    {
        if (framebuffer_count <= 0)
        {
            throw std::runtime_error("No Framebuffer count was specified");
        }

        // Decide on attachment index
        uint32_t attachment_index = std::max(last_attachment_index + 1, 0); // Default on the next index
        if (attachment_idx.has_value())
        {
            auto value = attachment_idx.value();
            // 1. Already existing location
            if (value <= last_attachment_index)
            {
                attachment_index = value;
            }
        }

        // Decide on framebuffer range
        uint32_t first = 0;
        uint32_t last  = framebuffer_count - 1;
        if (fb_idx.has_value())
        {
            first = last = fb_idx.value();
        }

        // Set attachment for framebuffers
        for (uint32_t i = first; i < last + 1; i++)
        {
            // New fb_idx -> Insert empty vector into map
            if (!attachments.contains(i))
            {
                attachments.insert({i, {}});
            }

            auto& vec = attachments[i];
            if (attachment_index <= vec.size())
            {
                vec.resize(attachment_index + 1);
            }

            vec[attachment_index] = image_view;

            std::cout << std::format("Set attachment #{} for framebuffer #{} : {}", attachment_index, i,
                                     std::to_string((uint64_t) vec[attachment_index].operator VkImageView())) << std::endl;
        }

        last_attachment_index = static_cast<int32_t>(attachment_index);
        return *this;
    }

    FramebufferCreateInfo& FramebufferCreateInfo::set_framebuffer_count(uint32_t value)
    {
        framebuffer_count = value;
        if (!attachments.empty())
        {
        #ifdef NVK_DEBUG
            std::cerr << "Setting the number of Framebuffers after adding attachments will reset the list of attachments" << std::endl;
            attachments.clear();
            last_attachment_index = -1;
        #else
            throw std::runtime_error("Call [FramebufferCreateInfo::set_framebuffer_count()] before adding attachments!");
        #endif
        }
        return *this;
    }

    FramebufferCreateInfo& FramebufferCreateInfo::validate()
    {
        bool has_missing_attachments = false;
        for (auto& [id, vec] : attachments)
        {
            if (!std::ranges::all_of(vec, [&](const auto& b){ return b; }))
            {
                has_missing_attachments = true;
            }
        }

        if (has_missing_attachments)
        {
            std::cerr << "Invalid FramebufferCreateInfo: Has missing attachments" << std::endl;
            for (auto& [id, vec] : attachments)
            {
            #ifdef NVK_DEBUG
                std::stringstream out;
                out << std::format("[Framebuffer #{}]", id);
                int32_t idx = -1;
                for (const auto& b : vec)
                {
                    out << std::format(" [{} : {}]", idx++, b ? "ok" : "missing");
                }
                std::cerr << out.str() << std::endl;
            #endif

                throw std::runtime_error("Missing attachments for Framebuffers");
            }
        }

        if (extent.width == 0 || extent.height == 0)
        {
        #ifdef NVK_DEBUG
            std::cerr << std::format("Invalid Extent for Framebuffers: [{}, {}]", extent.width, extent.height);
        #endif
            throw std::runtime_error("Invalid Extent for Framebuffers");
        }

        return *this;
    }

    Framebuffer::Framebuffer(FramebufferCreateInfo& create_info, const std::shared_ptr<Device>& device)
    : m_device(device)
    {
        auto fb_create_info = vk::FramebufferCreateInfo()
            .setAttachmentCount(create_info.last_attachment_index + 1)
            .setHeight(create_info.extent.height)
            .setLayers(1)
            .setRenderPass(create_info.render_pass)
            .setWidth(create_info.extent.width);

        m_framebuffers.resize(create_info.framebuffer_count);
        for (size_t i = 0; i < m_framebuffers.size(); i++)
        {
            fb_create_info.setPAttachments(create_info.attachments[i].data());
            if (vk::Result result = m_device->handle().createFramebuffer(&fb_create_info, nullptr, &m_framebuffers[i]);
                result != vk::Result::eSuccess)
            {
                throw std::runtime_error(std::format("Failed to create Framebuffer #{}", i));
            }
            m_device->name_object(std::format("[FB] {} #{}", create_info.name, i),
                                  (uint64_t) m_framebuffers[i].operator VkFramebuffer(),
                                  vk::ObjectType::eFramebuffer);
        }
    }

    Framebuffer::~Framebuffer()
    {
        if (!m_framebuffers.empty())
        {
            for (const auto& framebuffer : m_framebuffers)
            {
                m_device->handle().destroyFramebuffer(framebuffer);
            }
        }
    }

    const vk::Framebuffer& Framebuffer::get(size_t index)
    {
        if (index > m_framebuffers.size())
        {
            throw nlog::make_exception<std::out_of_range>("Index out of range for framebuffer array.");
        }

        return m_framebuffers[index];
    }

    const vk::Framebuffer& Framebuffer::operator[](size_t index)
    {
        return get(index);
    }

    const std::vector<vk::Framebuffer>& Framebuffer::framebuffers() const
    {
        return m_framebuffers;
    }

    size_t Framebuffer::count() const
    {
        return m_framebuffers.size();
    }
}