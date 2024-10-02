#include "Texture.hpp"

namespace Nebula::rl
{
    Texture::Texture(const TextureCreateInfo& create_info)
    : m_name(create_info.name)
    , m_path(create_info.path)
    , m_device(create_info.device)
    {
        int width, height, channels;
        stbi_uc* pixels = stbi_load(create_info.path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels)
        {
            throw std::runtime_error(std::format("Failed to load texture: {}", create_info.path));
        }
        vk::DeviceSize size = width * height * 4;

        auto staging_create_info = nvk::BufferCreateInfo()
            .set_buffer_type(nvk::BufferType::eStaging)
            .set_size(size);
        auto staging_buffer = nvk::Buffer::create_with_data(staging_create_info, pixels, m_device, create_info.command);

        stbi_image_free(pixels);

        auto image_create_info = nvk::ImageCreateInfo()
            .set_extent({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) })
            .set_format(vk::Format::eR8G8B8A8Srgb)
            .set_tiling(vk::ImageTiling::eOptimal)
            .set_name(create_info.name)
            .set_sample_count(vk::SampleCountFlagBits::e1)
            .set_usage_flags(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst)
            .set_with_sampler(true);
        m_image = nvk::Image::create(image_create_info, m_device);

        create_info.command->exec_single_time_command([&](const vk::CommandBuffer& cmd){
            nvk::ImageBarrier(m_image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal).apply(cmd);
            staging_buffer->copy_to_image(*m_image, cmd);
            nvk::ImageBarrier(m_image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal).apply(cmd);
        });

        m_desc_info = vk::DescriptorImageInfo()
            .setImageLayout(m_image->state().layout)
            .setImageView(m_image->image_view())
            .setSampler(m_image->default_sampler());
    }

    const vk::DescriptorImageInfo& Texture::get_descriptor_info() const
    {
        return m_desc_info;
    }

    const std::shared_ptr<nvk::Image>& Texture::image() const
    {
        return m_image;
    }
}