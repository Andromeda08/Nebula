#pragma once

#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>
#include "../Device.hpp"
#include "../Utility.hpp"

namespace Nebula::nvk
{
    struct ShaderCreateInfo
    {
        std::string             entry_point {"main"};
        std::string             file_path {};
        vk::ShaderStageFlagBits shader_stage {};

        ShaderCreateInfo() = default;

        inline ShaderCreateInfo& set_entry_point(const std::string& value = "main")
        {
            entry_point = value;
            return *this;
        }

        inline ShaderCreateInfo& set_file_path(const std::string& value)
        {
            file_path = value;
            return *this;
        }

        inline ShaderCreateInfo& set_shader_stage(vk::ShaderStageFlagBits value)
        {
            shader_stage = value;
            return *this;
        }
    };

    class Shader
    {
    public:
        NVK_DISABLE_COPY(Shader);

        Shader(const ShaderCreateInfo& create_info, const std::shared_ptr<Device>& device);

        vk::PipelineShaderStageCreateInfo make_stage_info() const;

    private:
        static std::vector<char> read_file(std::string const& file_path);

        vk::ShaderModule        m_shader;
        vk::ShaderStageFlagBits m_stage;
        std::string             m_entry_point;
        std::shared_ptr<Device> m_device;
    };
}
