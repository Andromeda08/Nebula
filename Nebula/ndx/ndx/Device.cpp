#include "Device.hpp"
#include "Utility.hpp"
#include <d3dx12.h>

namespace Nebula::ndx
{
    Device::Device(const Instance& instance)
    {
        select_device(instance.factory().Get());
        create_device();
        create_queues();
    }

    void Device::select_device(IDXGIFactory1* p_factory)
    {
        ComPtr<IDXGIAdapter1>& adapter = m_adapter;
        ComPtr<IDXGIFactory6> factory6;
        if (!SUCCEEDED(p_factory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            throw make_exception("Failed to create {}", red("IDXGIFactory6"));
        }

        for (
            uint32_t i = 0;
            SUCCEEDED(factory6->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)));
            ++i)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // Don't select software adapters
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }

        // If we couldn't find an adapter with `EnumAdapterByGpuPreference()`
        if (adapter.Get() == nullptr)
        {
            for (uint32_t i = 0; SUCCEEDED(p_factory->EnumAdapters1(i, &adapter)); ++i)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                // Don't select software adapters
                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }

                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }

        if (m_adapter.Get() == nullptr)
        {
            throw make_exception("Failed to find a suitable Adapter");
        }

        DXGI_ADAPTER_DESC1 desc;
        m_adapter->GetDesc1(&desc);
        m_name = to_str(desc.Description);

        pInfo("Selected Adapter: {}", m_name);
    }

    void Device::create_device()
    {
        // Required to run UNSIGNED precompile shaders
        D3D12EnableExperimentalFeatures(1, &D3D12ExperimentalShaderModels, nullptr, nullptr);

        if (auto result = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
            FAILED(result))
        {
            throw make_exception("Failed to create {}", red("D3D12Device"));
        }

        pInfo("Created {}", green("D3D12Device"));
    }

    void Device::create_queues()
    {
        D3D12_COMMAND_QUEUE_DESC qg_desc = {};
        qg_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        qg_desc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;

        if (auto result = m_device->CreateCommandQueue(&qg_desc, IID_PPV_ARGS(&m_graphics));
            FAILED(result))
        {
            throw make_exception("Failed to create {}, type: Direct", red("D3D12CommandQueue"));
        }

        pInfo("Created {}, type: Direct (Graphics)", green("D3D12CommandQueue"));

        D3D12_COMMAND_QUEUE_DESC qc_desc = {};
        qc_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        qc_desc.Type  = D3D12_COMMAND_LIST_TYPE_COMPUTE;

        if (auto result = m_device->CreateCommandQueue(&qg_desc, IID_PPV_ARGS(&m_compute));
            FAILED(result))
        {
            throw make_exception("Failed to create {}, type: Compute", red("D3D12CommandQueue"));
        }

        pInfo("Created {}, type: Compute", green("D3D12CommandQueue"));
    }

    void Device::make_root_signature(ID3D12RootSignature** pp_root_signature, D3D12_ROOT_SIGNATURE_FLAGS flags)
    {
        CD3DX12_ROOT_SIGNATURE_DESC root_signature_desc;
        root_signature_desc.Init(0, nullptr, 0, nullptr, flags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        if (auto result = D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
            FAILED(result))
        {
            throw make_exception("D3D12SerializeRootSignature() failed");
        }

        if (auto result = m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(pp_root_signature));
            FAILED(result))
        {
            throw make_exception("Failed to create {}", red("ID3D12RootSignature"));
        }
    }
}