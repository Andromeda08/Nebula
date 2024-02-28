#include "Image.hpp"
#include <format>
#include <stdexcept>

#ifdef NVK_VERBOSE_EXTRA
#include <iostream>
#endif

namespace Nebula::nvk
{
    Image::Image(const ImageCreateInfo& create_info, const std::shared_ptr<Device>& device)
    : m_device(device)
    , m_properties(Image::get_properties(create_info))
    , m_name(create_info.name)
    {
        auto img_create_info = vk::ImageCreateInfo()
            .setFormat(m_properties.format)
            .setExtent({ m_properties.extent.width, m_properties.extent.height, 1 })
            .setSamples(m_properties.sample_count)
            .setUsage(create_info.usage_flags)
            .setTiling(create_info.tiling)
            .setArrayLayers(1)
            .setMipLevels(1)
            .setImageType(vk::ImageType::e2D)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setInitialLayout(vk::ImageLayout::eUndefined);

        if (const vk::Result result = m_device->handle().createImage(&img_create_info, nullptr, &m_image);
            result != vk::Result::eSuccess)
        {
            throw std::runtime_error(std::format("Failed to create Image \"{}\" ({})", m_name, to_string(result)));
        }

        m_device->name_object(std::format("{}: Image", m_name),
                              (uint64_t)m_image.operator VkImage(),
                              vk::ObjectType::eImage);

        auto allocation_info = AllocationInfo()
            .set_image(m_image)
            .set_property_flags(create_info.memory_property_flags);

        m_allocation = m_device->allocate_memory(allocation_info);
        m_allocation->bind();

        auto view_create_info = vk::ImageViewCreateInfo()
            .setFormat(m_properties.format)
            .setImage(m_image)
            .setSubresourceRange(m_properties.subresource_range)
            .setViewType(vk::ImageViewType::e2D);

        if (const vk::Result result = m_device->handle().createImageView(&view_create_info, nullptr, &m_image_view);
            result != vk::Result::eSuccess)
        {
            throw std::runtime_error(std::format("Failed to create ImageView \"{}\" ({})", m_name, to_string(result)));
        }

        m_device->name_object(std::format("{}: ImageView", m_name),
                              (uint64_t)m_image_view.operator VkImageView(),
                              vk::ObjectType::eImageView);

        #ifdef NVK_VERBOSE_EXTRA
        std::cout << std::format("[V++] Created Image and ImageView: {}\n\tExtent: [{}x{}] | Format: {}",
                                 m_name, m_properties.extent.width, m_properties.extent.height,
                                 to_string(m_properties.format)) << std::endl;
        #endif
    }

    ImageProperties Image::get_properties(const ImageCreateInfo& create_info)
    {
        return ImageProperties {
            .format = create_info.format,
            .extent = create_info.extent,
            .sample_count = create_info.sample_count,
            .subresource_range = { create_info.aspect_flag, 0, 1, 0, 1 },
            .subresource_layers = { create_info.aspect_flag, 0, 0, 1 }
        };
    }

    Image::~Image()
    {
        m_device->handle().destroy(m_image_view);
        m_device->handle().destroy(m_image);
        m_allocation->free();

        #ifdef NVK_VERBOSE_EXTRA
        std::cout << std::format("[V++] Destroyed Image and ImageView: {}",m_name) << std::endl;
        #endif
    }
}