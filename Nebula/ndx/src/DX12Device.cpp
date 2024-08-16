#include "DX12Device.hpp"

namespace Nebula::ndx
{
    DX12Device::DX12Device(const DX12DeviceInfo& deviceInfo, IDXGIFactory1* pFactory)
    {
        selectDevice(pFactory);
        createDevice(deviceInfo);
        createQueues(deviceInfo);
    }

    void DX12Device::makeRootSignature(ID3D12RootSignature** ppRootSignature, const D3D12_ROOT_SIGNATURE_FLAGS flags) const
    {
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, flags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        NDX_CHECK(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error),
                  "Failed to serialize RootSignature");

        NDX_CHECK(mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(ppRootSignature)),
                  "Failed to create RootSignature");
    }

    void DX12Device::createCommandAllocator(const D3D12_COMMAND_LIST_TYPE type, ComPtr<ID3D12CommandAllocator>& commandAllocator) const
    {
        NDX_CHECK(mDevice->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)),
                  fmt::format("Failed to create CommandAllocator for CommandList type {}", to_string(type)));
    }

    void DX12Device::createCommandList(const uint32_t mask, const D3D12_COMMAND_LIST_TYPE type, const ComPtr<ID3D12CommandAllocator>& commandAllocator,
                                       ID3D12PipelineState* pPipelineState, ComPtr<ID3D12CommandList>& commandList) const
    {
        NDX_CHECK(mDevice->CreateCommandList(mask, type, commandAllocator.Get(), pPipelineState, IID_PPV_ARGS(&commandList)),
                  fmt::format("Failed to create {} CommandList", to_string(type)));
    }

    void DX12Device::createFence(const uint64_t initialValue, const D3D12_FENCE_FLAGS flags, ComPtr<ID3D12Fence>& fence) const
    {
        NDX_CHECK(mDevice->CreateFence(initialValue, flags, IID_PPV_ARGS(&fence)), "Failed to create Fence");
    }

    void DX12Device::createGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipelineStateDesc, ComPtr<ID3D12PipelineState>& pipelineState) const
    {
        NDX_CHECK(mDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineState)), "Failed to create GraphicsPipelineState");
    }

    std::shared_ptr<DX12Device> DX12Device::createDX12Device(const DX12DeviceInfo& deviceInfo, IDXGIFactory1* pFactory)
    {
        return std::make_shared<DX12Device>(deviceInfo, pFactory);
    }

    void DX12Device::selectDevice(IDXGIFactory1* pFactory)
    {
        ComPtr<IDXGIAdapter1>& adapter = mAdapter;
        ComPtr<IDXGIFactory6> factory;

        if (!SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory))))
        {
            NDX_THROW("Failed to get Factory6");
        }

        for (uint32_t i = 0;
             SUCCEEDED(factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)));
             ++i)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }

        if (adapter.Get() == nullptr)
        {
            for (uint32_t i = 0; SUCCEEDED(pFactory->EnumAdapters1(i, &adapter)); ++i)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }

                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }

        if (mAdapter.Get() == nullptr)
        {
            NDX_THROW("Failed to find a suitable Adapter");
        }

        mAdapter->GetDesc1(&mDesc1);
        mName = to_string(mDesc1.Description);

        NDX_IF_DEBUG(fmt::println(NDX_OK(fmt::format("Selected Adapter: {}", mName))));
    }

    void DX12Device::createDevice(const DX12DeviceInfo& deviceInfo)
    {
        NDX_CHECK(D3D12CreateDevice(mAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mDevice)), "Failed to create D3D12Device");
        NDX_IF_DEBUG(fmt::println(NDX_OK("Created D3D12Device")));
    }

    void DX12Device::createQueues(const DX12DeviceInfo& deviceInfo)
    {
        D3D12_COMMAND_QUEUE_DESC qGeneralDesc = {};
        qGeneralDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        qGeneralDesc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;

        NDX_CHECK(mDevice->CreateCommandQueue(&qGeneralDesc, IID_PPV_ARGS(&mQueueDirect)),
                  fmt::format("Failed to create {} D3D12CommandQueue", fmt::styled(to_string(qGeneralDesc.Type), fmt::fg(to_color(qGeneralDesc.Type)))));
        NDX_IF_DEBUG(fmt::println(NDX_OK(fmt::format("Created {} D3D12CommandQueue", fmt::styled(to_string(qGeneralDesc.Type), fmt::fg(to_color(qGeneralDesc.Type)))))));

        D3D12_COMMAND_QUEUE_DESC qCopyDesc = {};
        qCopyDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        qCopyDesc.Type  = D3D12_COMMAND_LIST_TYPE_COPY;

        NDX_CHECK(mDevice->CreateCommandQueue(&qCopyDesc, IID_PPV_ARGS(&mQueueCopy)),
                  fmt::format("Failed to create {} D3D12CommandQueue", fmt::styled(to_string(qCopyDesc.Type), fmt::fg(to_color(qCopyDesc.Type)))));
        NDX_IF_DEBUG(fmt::println(NDX_OK(fmt::format("Created {} D3D12CommandQueue", fmt::styled(to_string(qCopyDesc.Type), fmt::fg(to_color(qCopyDesc.Type)))))));

        try
        {
            D3D12_COMMAND_QUEUE_DESC qComputeDesc = {};
            qComputeDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            qComputeDesc.Type  = D3D12_COMMAND_LIST_TYPE_COMPUTE;

            NDX_CHECK(mDevice->CreateCommandQueue(&qComputeDesc, IID_PPV_ARGS(&mQueueCompute)),
                      fmt::format("Failed to create {} D3D12CommandQueue", fmt::styled(to_string(qComputeDesc.Type), fmt::fg(to_color(qComputeDesc.Type)))));
            NDX_IF_DEBUG(fmt::println(NDX_OK(fmt::format("Created {} D3D12CommandQueue", fmt::styled(to_string(qComputeDesc.Type), fmt::fg(to_color(qComputeDesc.Type)))))));
        }
        catch (const std::runtime_error& e)
        {
            if (deviceInfo.requireDedicatedComputeQueue)
            {
                throw;
            }
        }
    }
}
