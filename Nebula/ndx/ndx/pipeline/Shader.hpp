#pragma once
#include <memory>
#include <string>
#include <wrl/client.h>
#include <d3d12.h>
#include "../Device.hpp"
#include "../Struct.hpp"

using Microsoft::WRL::ComPtr;
namespace Nebula::ndx
{
    enum class ShaderStage
    {
        eVertex,
        eTesselationControl,
        eTesselationEval,
        eGeometry,
        eFragment,
        eCompute,
        eRayGen,
        eClosestHit,
        eMiss,
        eInvokable,
        eMesh,
        eTask
    };

    struct ShaderCreateInfo
    {
        struct_param(ShaderCreateInfo, ShaderStage, shader_stage, ShaderStage::eVertex);
        struct_param(ShaderCreateInfo, std::string, file_path, {});
        struct_param(ShaderCreateInfo, std::string, entry_point, "main");
    };

    class Shader
    {
    public:
        DISABLE_COPY(Shader);

        Shader(const ShaderCreateInfo& create_info, const std::shared_ptr<Device>& device);

        ID3DBlob* handle() { return m_shader_blob.Get(); }

    private:
        ComPtr<ID3DBlob>    m_shader_blob;
        ShaderStage         m_shader_stage;
        std::string         m_entry_point;
    };
}