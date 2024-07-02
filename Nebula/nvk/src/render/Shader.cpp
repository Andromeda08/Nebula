#include "render/Shader.hpp"
#include "Utilities.hpp"
#include <fstream>

namespace Nebula::nvk
{
    Shader::Shader(const ShaderCreateInfo& create_info, const std::shared_ptr<Device>& device)
    : m_device(device), m_stage(create_info.shader_stage), m_entry_point(create_info.entry_point)
    {
        auto shader_source_code = Shader::read_file(create_info.file_path);

        auto sh_create_info = vk::ShaderModuleCreateInfo()
            .setCodeSize(sizeof(char) * shader_source_code.size())
            .setPCode(reinterpret_cast<const uint32_t*>(shader_source_code.data()));

        if (const vk::Result result = m_device->handle().createShaderModule(&sh_create_info, nullptr, &m_shader);
            result != vk::Result::eSuccess)
        {
            throw make_exception("Failed to create Shader from file: {} ()", create_info.file_path, to_string(result));
        }

        print_success("Created Shader \"{}\" (Stage: {})", create_info.file_path, to_string(m_stage));
    }

    vk::PipelineShaderStageCreateInfo Shader::make_stage_info() const
    {
        return vk::PipelineShaderStageCreateInfo()
            .setStage(m_stage)
            .setModule(m_shader)
            .setPName(m_entry_point.c_str());
    }

    std::vector<char> Shader::read_file(const std::string& file_path)
    {
        std::ifstream file(file_path, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw make_exception("Failed to open file: {}!", file_path);
        }

        size_t file_size = static_cast<size_t> (file.tellg());
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();
        return buffer;
    }
}