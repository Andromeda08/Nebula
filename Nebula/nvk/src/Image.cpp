#include "Image.hpp"
#include <format>
#include <stdexcept>
#include "Barrier.hpp"
#include "Utilities.hpp"

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
            throw make_exception("Failed to create Image \"{}\" ({})", m_name, to_string(result));
        }

        m_device->name_object(m_image, std::format("{} [Image]", m_name), vk::ObjectType::eImage);

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
            throw make_exception("Failed to create ImageView \"{}\" ({})", m_name, to_string(result));
        }

        m_device->name_object(m_image_view, std::format("{} [ImageView]", m_name), vk::ObjectType::eImageView);

        if (create_info.with_sampler)
        {
            auto sampler_create_info = vk::SamplerCreateInfo()
                .setMagFilter(vk::Filter::eLinear)
                .setMinFilter(vk::Filter::eLinear)
                .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                .setAnisotropyEnable(false)
                .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                .setUnnormalizedCoordinates(false)
                .setCompareEnable(false)
                .setCompareOp(vk::CompareOp::eAlways)
                .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                .setMipLodBias(0.0f)
                .setMinLod(0.0f)
                .setMaxLod(0.0f);

            if (const vk::Result result = m_device->handle().createSampler(&sampler_create_info, nullptr, &m_sampler);
                result != vk::Result::eSuccess)
            {
                throw make_exception("Failed to create Sampler for Image \"{}\" ({})", m_name, to_string(result));
            }

            m_device->name_object(m_sampler, std::format("{} [Sampler]", m_name), vk::ObjectType::eSampler);
        }

        print_verbose("[Created Image and ImageView: {}\n\tExtent: [{}x{}] | Format: {}",
                      m_name, m_properties.extent.width, m_properties.extent.height,
                      to_string(m_properties.format));
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

        print_verbose("Destroyed Image and ImageView: {}", m_name);
    }

    ImageBlit& ImageBlit::set_src_image(const std::shared_ptr<Image>& src_image)
    {
        m_src_image = src_image;
        auto barrier = ImageBarrier(m_src_image,m_src_image->state().layout,vk::ImageLayout::eTransferSrcOptimal);
        m_barriers[0] = barrier.barrier();

        m_image_blit
            .setSrcOffsets({0, 0})
            .setSrcSubresource(m_src_image->properties().subresource_layers);

        m_blit_image_info
            .setSrcImageLayout(vk::ImageLayout::eTransferSrcOptimal)
            .setSrcImage(m_src_image->image());

        return *this;
    }

    ImageBlit& ImageBlit::set_dst_image(const std::shared_ptr<Image>& dst_image)
    {
        m_dst_image = dst_image;
        auto barrier = ImageBarrier(m_dst_image,m_dst_image->state().layout,vk::ImageLayout::eTransferDstOptimal);
        m_barriers[1] = barrier.barrier();

        m_image_blit
            .setDstOffsets({0, 0})
            .setDstSubresource(m_dst_image->properties().subresource_layers);

        m_blit_image_info
            .setDstImageLayout(vk::ImageLayout::eTransferSrcOptimal)
            .setDstImage(m_dst_image->image());

        return *this;
    }

    void ImageBlit::blit(const vk::CommandBuffer& command_buffer)
    {
        m_blit_image_info
            .setFilter(vk::Filter::eNearest)
            .setRegionCount(1)
            .setPRegions(&m_image_blit);

        m_dependency_info
            .setImageMemoryBarrierCount(2)
            .setPImageMemoryBarriers(m_barriers.data());

        command_buffer.pipelineBarrier2(m_dependency_info);
        command_buffer.blitImage2(&m_blit_image_info);

        m_src_image->update_state({ vk::AccessFlagBits2::eNone, m_barriers[0].newLayout });
        m_dst_image->update_state({ vk::AccessFlagBits2::eNone, m_barriers[1].newLayout });
    }
}