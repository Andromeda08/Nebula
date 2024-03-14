#include "Swapchain.hpp"

#include <format>
#include <iostream>
#include <stdexcept>
#include "Utility.hpp"

namespace Nebula::ndx
{
    Swapchain::Swapchain(const std::shared_ptr<IWindow>& window, const std::shared_ptr<Context>& context)
    {
        auto size = window->framebuffer_size();

        DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
        swapchain_desc.BufferCount = 2;
        swapchain_desc.Width = size.width;
        swapchain_desc.Height = size.height;
        swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.SampleDesc.Count = 1;

        ComPtr<IDXGIFactory4>   factory = context->factory();
        std::shared_ptr<Device> device  = context->device();

        ComPtr<IDXGISwapChain1> swapchain;
        auto res = factory->CreateSwapChainForHwnd(
            device->get_command_queue().Get(), window->get_win32_handle(),
            &swapchain_desc, nullptr, nullptr,
            &swapchain);

        if (FAILED(res))
        {
            throw std::runtime_error(std::format("{} Failed to create {}", p_error, green("DXGISwapChain")));
        }

        swapchain.As(&m_swapchain);
        m_frame_index = m_swapchain->GetCurrentBackBufferIndex();

        std::cout << std::format("{} Created {}", p_info, green("DXGISwapChain"));
    }
}