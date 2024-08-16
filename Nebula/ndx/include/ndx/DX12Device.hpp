#pragma once

#include "core.hpp"

namespace Nebula::ndx
{
    class DX12Device
    {
    public:
        NDX_DISABLE_COPY(DX12Device);
        NDX_DEF_PRIMARY_CTOR(DX12Device, const DX12DeviceInfo& deviceInfo, IDXGIFactory1* pFactory);

        void makeRootSignature(ID3D12RootSignature** ppRootSignature, D3D12_ROOT_SIGNATURE_FLAGS flags) const;

        void createCommandAllocator(D3D12_COMMAND_LIST_TYPE type, ComPtr<ID3D12CommandAllocator>& commandAllocator) const;
        void createCommandList(uint32_t mask, D3D12_COMMAND_LIST_TYPE type, const ComPtr<ID3D12CommandAllocator>& commandAllocator, ID3D12PipelineState* pPipelineState, ComPtr<ID3D12CommandList>& commandList) const;

        void createFence(uint64_t initialValue, D3D12_FENCE_FLAGS flags, ComPtr<ID3D12Fence>& fence) const;

        void createGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipelineStateDesc, ComPtr<ID3D12PipelineState>& pipelineState) const;

        const DX12Queue& queueCompute() const { return mQueueCompute; }
        const DX12Queue& queueCopy()    const { return mQueueCopy;    }
        const DX12Queue& queueDirect()  const { return mQueueDirect; }

        const ComPtr<ID3D12Device>& handle() const { return mDevice; }

        const std::string& name() const { return mName; }

    private:
        void selectDevice(IDXGIFactory1* pFactory);
        void createDevice(const DX12DeviceInfo& deviceInfo);
        void createQueues(const DX12DeviceInfo& deviceInfo);

        ComPtr<IDXGIAdapter1> mAdapter;
        ComPtr<ID3D12Device>  mDevice;

        DX12Queue             mQueueCompute;
        DX12Queue             mQueueCopy;
        DX12Queue             mQueueDirect;

        std::string           mName  {"No Adapter"};
        DXGI_ADAPTER_DESC1    mDesc1 {};
    };
}