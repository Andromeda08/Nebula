#pragma once

#include "core.hpp"

namespace Nebula::ndx
{
    class DX12Pipeline
    {
    public:
        NDX_DISABLE_COPY(DX12Pipeline);
        NDX_DEF_PRIMARY_CTOR(DX12Pipeline, const DX12PipelineInfo& pipelineInfo, const DX12DevicePtr& device);

        void bind(const ComPtr<ID3D12GraphicsCommandList>& graphicsCommandList) const;

    private:
        static D3D12_ROOT_SIGNATURE_FLAGS getRootSignatureFlags(bool hasVertexInputs);

        static void assignPipelineStateDescShaders(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const std::vector<DX12ShaderPtr>& shaders);

        ComPtr<ID3D12RootSignature> mRootSignature;
        ComPtr<ID3D12PipelineState> mPSO;
        PipelineType                mType;
        std::string                 mName;
        DX12DevicePtr               mDevice;
    };
}
