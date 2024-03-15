#include "Swapchain.hpp"
#include "Utility.hpp"
#include <d3dx12.h>

namespace Nebula::ndx
{
    Swapchain::Swapchain(const SwapchainCreateInfo& create_info)
    : m_frame_count(create_info.frame_count)
    {
        auto& window    = create_info.window;
        auto& instance  = create_info.instance;
        auto& device    = create_info.device;

        auto size = window->framebuffer_size();
        m_size = size;

        DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
        swapchain_desc.BufferCount      = m_frame_count;
        swapchain_desc.Width            = size.width;
        swapchain_desc.Height           = size.height;
        swapchain_desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchain_desc.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapchain_desc.SampleDesc.Count = 1;

        ComPtr<IDXGIFactory4> factory = instance->factory();

        ComPtr<IDXGISwapChain1> swapchain;
        auto create_result = factory->CreateSwapChainForHwnd(
            device->qGraphics().Get(), window->get_win32_handle(),
            &swapchain_desc, nullptr, nullptr,
            &swapchain);

        if (FAILED(create_result))
        {
            throw make_exception("Failed to create {}", red("DXGISwapChain"));
        }

        swapchain.As(&m_swapchain);
        m_frame_index = m_swapchain->GetCurrentBackBufferIndex();

        std::cout << std::format("{} Created {}", p_info, green("DXGISwapChain")) << std::endl;

        // Create RTVs
        D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
        rtv_heap_desc.NumDescriptors = m_frame_count;
        rtv_heap_desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtv_heap_desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        if (auto result = device->handle()->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&m_rtv_heap));
            FAILED(result))
        {
            throw make_exception("Failed to create {} for {}", red("D3D12DescriptorHeap"), green("DXGISwapChain"));
        }

        m_rtv_size = device->handle()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());
        m_render_targets.resize(m_frame_count);
        for (uint32_t i = 0; i < m_frame_count; i++)
        {
             if (auto result = m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_render_targets[i]));
                FAILED(result))
             {
                 throw make_exception("Failed to get Swapchain Buffer #{}", i);
             }
             device->handle()->CreateRenderTargetView(m_render_targets[i].Get(), nullptr, rtv_handle);
             rtv_handle.Offset(1, m_rtv_size);
        }

        pInfo("Created {} Swapchain RTVs", cyan(std::to_string(m_frame_count)));
    }

    uint32_t Swapchain::get_current_back_buffer_index() const
    {
        return m_swapchain->GetCurrentBackBufferIndex();
    }

    void Swapchain::present(uint32_t sync_interval, uint32_t flags) const
    {
        if (auto result = m_swapchain->Present(sync_interval, flags);
            FAILED(result))
        {
            throw make_exception("Present call to Swapchain failed");
        }
    }

    D3D12_CPU_DESCRIPTOR_HANDLE Swapchain::get_rtv_handle(uint32_t i) const
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(i), m_rtv_size);
        return rtv_handle;
    }

    void Swapchain::set_viewport_scissor(const ComPtr<ID3D12GraphicsCommandList>& command_list) const
    {
        CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_size.width), static_cast<float>(m_size.height));
        CD3DX12_RECT scissor  = CD3DX12_RECT(0, 0, static_cast<long>(m_size.width), static_cast<long>(m_size.height));

        command_list->RSSetViewports(1, &viewport);
        command_list->RSSetScissorRects(1, &scissor);
    }
}