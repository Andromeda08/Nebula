#include "DX12Shader.hpp"

namespace Nebula::ndx
{
    DX12Shader::DX12Shader(const DX12ShaderInfo& shaderInfo)
    : mShaderStage(shaderInfo.shaderStage)
    {
        const auto path = NDX_TO_WSTR(shaderInfo.filePath);
        NDX_CHECK(D3DReadFileToBlob(path.c_str(), &mShaderBlob), fmt::format("Failed to read shader bytecode from file: {}", shaderInfo.filePath));
    }

    std::shared_ptr<DX12Shader> DX12Shader::createDX12Shader(const DX12ShaderInfo& shaderInfo)
    {
        return std::make_shared<DX12Shader>(shaderInfo);
    }

    ComPtr<ID3DBlob>& DX12Shader::handle()
    {
        return mShaderBlob;
    }

    ShaderStage DX12Shader::shaderStage() const
    {
        return mShaderStage;
    }
}
