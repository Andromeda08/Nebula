#pragma once
#include <memory>
#include <string>
#include <wrl/client.h>
#include <d3d12.h>
#include "../Device.hpp"
#include "../_rhi.hpp"

using Microsoft::WRL::ComPtr;
namespace Nebula::ndx
{
    class Shader
    {
    public:
        DISABLE_COPY(Shader);

        Shader(const ShaderCreateInfo& create_info, const std::shared_ptr<Device>& device);

        ID3DBlob* handle() { return m_shader_blob.Get(); }

        ShaderStage get_shader_stage() const { return m_shader_stage; }

    private:
        ComPtr<ID3DBlob> m_shader_blob;
        ShaderStage      m_shader_stage {ShaderStage::eVertex};
    };
}