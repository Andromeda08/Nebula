#pragma once

#include <string>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;

namespace Nebula::ndx
{
    class Device
    {
    public:
        explicit Device(IDXGIFactory1* p_factory);

        std::string get_adapter_name() const;

        const ComPtr<ID3D12CommandQueue>& get_command_queue() const { return m_command_queue; }

        const ComPtr<ID3D12Device>& handle() const { return m_device; }

    private:
        void select_device(IDXGIFactory1* p_factory);

        void create_device();

        ComPtr<IDXGIAdapter1>   m_adapter;
        ComPtr<ID3D12Device>    m_device;

        ComPtr<ID3D12CommandQueue> m_command_queue;
    };
}