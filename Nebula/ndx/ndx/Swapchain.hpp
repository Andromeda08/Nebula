#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "Context.hpp"
#include "IWindow.hpp"

using namespace Microsoft::WRL;

namespace Nebula::ndx
{
    class Swapchain
    {
    public:
        Swapchain(const std::shared_ptr<IWindow>& window, const std::shared_ptr<Context>& context);

    private:
        ComPtr<IDXGISwapChain3> m_swapchain;
        uint32_t                m_frame_index;
    };
}