#pragma once

#include <memory>
#include <string>
#include <nvk/Device.hpp>
#include <nvk/Image.hpp>

namespace Nebula::rl
{
    struct TextureCreateInfo
    {
        std::string name;
        std::string path;
        std::shared_ptr<nvk::Device> device;
    };

    class Texture
    {
    public:
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        explicit Texture(const TextureCreateInfo& create_info);

    private:
        std::string                  m_name;
        std::string                  m_file;
        std::shared_ptr<nvk::Image>  m_image;
        std::shared_ptr<nvk::Device> m_device;
    };
}