#include "Shader.hpp"
#include <d3dcompiler.h>
#include "../Utility.hpp"

namespace Nebula::ndx
{
    Shader::Shader(const ShaderCreateInfo& create_info, const std::shared_ptr<Device>& device)
    {
        auto path = to_wstr(create_info.file_path);
        if (auto result = D3DReadFileToBlob(path.c_str(), &m_shader_blob);
            FAILED(result))
        {
            throw make_exception("Failed to read shader bytecode from file: {}", create_info.file_path);
        }
    }
}