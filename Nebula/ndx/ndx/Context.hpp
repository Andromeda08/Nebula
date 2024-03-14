#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "Device.hpp"

using namespace Microsoft::WRL;

namespace Nebula::ndx
{
    class Context
    {
    public:
        Context();

        const ComPtr<IDXGIFactory4>& factory() const { return m_factory; }

        const std::shared_ptr<Device>& device() const { return m_device; }

    private:
        ComPtr<IDXGIFactory4>   m_factory;
        std::shared_ptr<Device> m_device;
    };
}