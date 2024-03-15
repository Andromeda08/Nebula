#pragma once

#include <string>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "Instance.hpp"

using Microsoft::WRL::ComPtr;

namespace Nebula::ndx
{
    using Queue = ComPtr<ID3D12CommandQueue>;

    class Device
    {
    public:
        explicit Device(const Instance& instance);

        // Command queues
        const Queue& qGraphics() const { return m_graphics; }
        const Queue& qCompute() const { return m_compute; }

        // D3D12 Stuff
        void make_root_signature(ID3D12RootSignature** pp_root_signature, D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

        const ComPtr<ID3D12Device>& handle() const { return m_device; }
        const std::string& name() const { return m_name; }

    private:
        void select_device(IDXGIFactory1* p_factory);

        void create_device();

        void create_queues();

        ComPtr<IDXGIAdapter1> m_adapter;
        ComPtr<ID3D12Device>  m_device;

        Queue                 m_graphics;
        Queue                 m_compute;

        std::string           m_name;
    };
}