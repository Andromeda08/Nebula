#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <d3d12.h>
#include <wrl/client.h>
#include "../Device.hpp"
#include "../_rhi.hpp"

namespace Nebula::ndx
{
    using Microsoft::WRL::ComPtr;

    class Pipeline
    {
    public:
        Pipeline(const PipelineCreateInfo& create_info, const std::shared_ptr<Device>& device);

        void bind(const ComPtr<ID3D12GraphicsCommandList>& command_list);

    private:
        void create_root_signature(const PipelineCreateInfo& create_info);

        static void assign_shaders(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const std::vector<std::shared_ptr<Shader>>& shaders);

        ComPtr<ID3D12RootSignature> m_root_signature;
        ComPtr<ID3D12PipelineState> m_pso;
        PipelineType                m_type{PipelineType::eUnknown};

        std::shared_ptr<Device>     m_device;
    };
}