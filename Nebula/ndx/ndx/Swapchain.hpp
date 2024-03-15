#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d12.h>
#include "Instance.hpp"
#include "Device.hpp"
#include "IWindow.hpp"

using namespace Microsoft::WRL;

namespace Nebula::ndx
{
    struct SwapchainCreateInfo
    {
        uint32_t                  frame_count {2};
        std::shared_ptr<IWindow>  window;
        std::shared_ptr<Instance> instance;
        std::shared_ptr<Device>   device;
    };

    class Swapchain
    {
    public:
        explicit Swapchain(const SwapchainCreateInfo& create_info);

        uint32_t get_current_back_buffer_index() const;

        void present(uint32_t sync_interval = 1, uint32_t flags = 0) const;

        void set_viewport_scissor(const ComPtr<ID3D12GraphicsCommandList>& command_list) const;

        D3D12_CPU_DESCRIPTOR_HANDLE get_rtv_handle(uint32_t i) const;

        const ComPtr<ID3D12Resource>& get_rtv(uint32_t i) const
        {
            return m_render_targets[i];
        }

    private:
        ComPtr<ID3D12DescriptorHeap>        m_rtv_heap;
        uint32_t                            m_rtv_size;
        std::vector<ComPtr<ID3D12Resource>> m_render_targets;

        ComPtr<IDXGISwapChain3>             m_swapchain;

        Size                                m_size {};
        uint32_t                            m_frame_count;
        uint32_t                            m_frame_index;
    };
}