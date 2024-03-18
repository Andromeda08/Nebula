#include "Shader.hpp"
#include "../Utility.hpp"
#include <d3dcompiler.h>

namespace Nebula::ndx
{
    Shader::Shader(const ShaderCreateInfo& create_info, const std::shared_ptr<Device>& device)
    : m_shader_stage(create_info.shader_stage), m_entry_point(create_info.entry_point)
    {
        auto path = to_wstr(create_info.file_path);
        if (auto result = D3DReadFileToBlob(path.c_str(), &m_shader_blob);
            FAILED(result))
        {
            throw make_exception("Failed to read shader bytecode from file: {}", create_info.file_path);
        }
    }
}