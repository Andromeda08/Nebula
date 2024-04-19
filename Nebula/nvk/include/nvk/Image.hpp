#pragma once

#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>
#include "Device.hpp"
#include "Utility.hpp"

namespace Nebula::nvk
{
    struct ImageCreateInfo
    {
        vk::ImageAspectFlagBits aspect_flag {vk::ImageAspectFlagBits::eColor};
        vk::Extent2D            extent {400, 400};
        vk::Format              format {vk::Format::eR32G32B32A32Sfloat};
        vk::MemoryPropertyFlags memory_property_flags {vk::MemoryPropertyFlagBits::eDeviceLocal};
        std::string             name {"Unknown"};
        vk::SampleCountFlagBits sample_count {vk::SampleCountFlagBits::e1};
        vk::ImageTiling         tiling {vk::ImageTiling::eOptimal};
        vk::ImageUsageFlags     usage_flags {vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled};

        struct_param(bool, with_sampler, false);

        ImageCreateInfo() = default;

        inline ImageCreateInfo& set_aspect_flags(vk::ImageAspectFlagBits value)
        {
            aspect_flag = value;
            return *this;
        }

        inline ImageCreateInfo& set_extent(vk::Extent2D value)
        {
            extent = value;
            return *this;
        }

        inline ImageCreateInfo& set_format(vk::Format value)
        {
            format = value;
            return *this;
        }

        inline ImageCreateInfo& set_memory_property_flags(vk::MemoryPropertyFlags value)
        {
            memory_property_flags = value;
            return *this;
        }

        inline ImageCreateInfo& set_name(const std::string& value)
        {
            name = value;
            return *this;
        }

        inline ImageCreateInfo& set_sample_count(vk::SampleCountFlagBits value)
        {
            sample_count = value;
            return *this;
        }

        inline ImageCreateInfo& set_tiling(vk::ImageTiling value)
        {
            tiling = value;
            return *this;
        }

        inline ImageCreateInfo& set_usage_flags(vk::ImageUsageFlags value)
        {
            usage_flags = value;
            return *this;
        }

    };

    struct ImageProperties
    {
        vk::Format                 format { vk::Format::eR32G32B32Sfloat };
        vk::Extent2D               extent { 1920, 1080 };
        vk::SampleCountFlagBits    sample_count { vk::SampleCountFlagBits::e1 };
        vk::ImageSubresourceRange  subresource_range { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
        vk::ImageSubresourceLayers subresource_layers { vk::ImageAspectFlagBits::eColor, 0, 0, 1 };
    };

    struct ImageState
    {
        vk::AccessFlags2 access_flags { vk::AccessFlagBits2::eNone };
        vk::ImageLayout  layout { vk::ImageLayout::eUndefined };
    };

    class Image
    {
    public:
        NVK_DISABLE_COPY(Image);

        Image(const ImageCreateInfo& create_info, const std::shared_ptr<Device>& device);

        ~Image();

        const vk::Image& image() const { return m_image; }

        const vk::ImageView& image_view() const { return m_image_view; }

        const ImageProperties& properties() const { return m_properties; }

        const ImageState& state() const { return m_state; }

        void update_state(const ImageState& state) { m_state = state; }

        const vk::Sampler& default_sampler() const { return m_sampler; }

        static inline std::shared_ptr<Image> create(const ImageCreateInfo& create_info,
                                                    const std::shared_ptr<Device>& device)
        {
            return std::make_shared<Image>(create_info, device);
        }

    private:
        static ImageProperties get_properties(const ImageCreateInfo& create_info);

        std::shared_ptr<Allocation> m_allocation;
        vk::Image                   m_image;
        vk::ImageView               m_image_view;
        vk::Sampler                 m_sampler;
        ImageState                  m_state;

        std::shared_ptr<Device>     m_device;
        const std::string           m_name;
        const ImageProperties       m_properties;
    };

    struct ImageBlit
    {
        ImageBlit& set_src_image(const std::shared_ptr<Image>& src_image);

        ImageBlit& set_dst_image(const std::shared_ptr<Image>& dst_image);

        void blit(const vk::CommandBuffer& command_buffer);

    private:
        vk::ImageBlit2                         m_image_blit;
        vk::BlitImageInfo2                     m_blit_image_info;
        std::array<vk::ImageMemoryBarrier2, 2> m_barriers;
        vk::DependencyInfo                     m_dependency_info;
        std::shared_ptr<Image>                 m_src_image;
        std::shared_ptr<Image>                 m_dst_image;
    };
}