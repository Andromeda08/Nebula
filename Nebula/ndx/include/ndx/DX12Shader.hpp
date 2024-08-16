#pragma once

#include "core.hpp"

namespace Nebula::ndx
{
    class DX12Shader
    {
    public:
        NDX_DISABLE_COPY(DX12Shader);
        explicit NDX_DEF_PRIMARY_CTOR(DX12Shader, const DX12ShaderInfo& shaderInfo);

        ComPtr<ID3DBlob>& handle();

        ShaderStage shaderStage() const;

    private:
        ComPtr<ID3DBlob> mShaderBlob;
        ShaderStage      mShaderStage;
    };
}