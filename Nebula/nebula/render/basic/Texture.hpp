#pragma once

#include <format>
#include <memory>
#include <string>
#include <stb_image.h>
#include <vulkan/vulkan.hpp>
#include <nvk/Barrier.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Command.hpp>
#include <nvk/Device.hpp>
#include <nvk/Image.hpp>

namespace Nebula::rl
{
    struct TextureCreateInfo
    {
        std::string name;
        std::string path;
        std::shared_ptr<nvk::Device> device;
        std::shared_ptr<nvk::CommandPool> command;
    };

    class Texture
    {
    public:
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        explicit Texture(const TextureCreateInfo& create_info);

        const vk::DescriptorImageInfo& get_descriptor_info() const;

        const std::shared_ptr<nvk::Image>& image() const;

    private:
        vk::DescriptorImageInfo      m_desc_info;

        std::string                  m_name;
        std::string                  m_path;
        std::shared_ptr<nvk::Image>  m_image;
        std::shared_ptr<nvk::Device> m_device;
    };
}