#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;

namespace Nebula::ndx
{
    class Instance
    {
    public:
        explicit Instance(bool debug = false);

        ComPtr<IDXGIFactory4> factory() const { return m_factory; }

    private:
        ComPtr<ID3D12Debug>   m_debug;
        ComPtr<IDXGIFactory4> m_factory;
    };
}