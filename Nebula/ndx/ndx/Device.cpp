#include "Device.hpp"

#include <format>
#include <iostream>
#include <stdexcept>
#include "Utility.hpp"

namespace Nebula::ndx
{
    Device::Device(IDXGIFactory1* p_factory)
    {
        select_device(p_factory);
        create_device();

        D3D12_COMMAND_QUEUE_DESC cq_desc = {};
        cq_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        cq_desc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;

        if (auto result = m_device->CreateCommandQueue(&cq_desc, IID_PPV_ARGS(&m_command_queue));
            FAILED(result))
        {
            throw std::runtime_error(std::format("{} Failed to create {}", p_error, green("D3D12CommandQueue")));
        }

        std::cout << std::format("{} Created {}", p_info, green("D3D12CommandQueue")) << std::endl;
    }

    std::string Device::get_adapter_name() const
    {
        if (m_adapter.Get() == nullptr)
        {
            throw std::runtime_error(std::format("{} No Adapter selected", p_error));
        }

        DXGI_ADAPTER_DESC1 desc;
        m_adapter->GetDesc1(&desc);
        return to_str(desc.Description);
    }

    void Device::select_device(IDXGIFactory1* p_factory)
    {
        ComPtr<IDXGIAdapter1>& adapter = m_adapter;
        ComPtr<IDXGIFactory6> factory6;
        if (!SUCCEEDED(p_factory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            throw std::runtime_error("Failed to create IDXGIFactory6");
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
            throw std::runtime_error(std::format("{} Failed to find a suitable Adapter", p_error));
        }

        std::cout << std::format("{} Selected Adapter: {}", p_info, get_adapter_name()) << std::endl;
    }

    void Device::create_device()
    {
        if (auto result = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
            FAILED(result))
        {
            throw std::runtime_error(std::format("{} Failed to create {}", p_error, green("D3D12Device")));
        }

        std::cout << std::format("{} Created {}", p_info, green("D3D12Device")) << std::endl;
    }
}