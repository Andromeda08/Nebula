#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <wrl/client.h>
#include "IWindow.hpp"
#include "Instance.hpp"
#include "Device.hpp"
#include "Swapchain.hpp"

using Microsoft::WRL::ComPtr;
namespace Nebula::ndx
{
    struct ContextCreateInfo
    {
        uint32_t                 frame_count {2};
        std::shared_ptr<IWindow> window;
    };

    class Context
    {
    public:
        explicit Context(const ContextCreateInfo& create_info);

        void on_render();

        // "WaitForGPU"
        void wait_idle();

        void move_to_next_frame();

    private:
        std::shared_ptr<Instance>   m_instance;
        std::shared_ptr<Device>     m_device;
        std::shared_ptr<Swapchain>  m_swapchain;

        // D3D12 CommandAllocator and Lists (For Graphics)
        std::vector<ComPtr<ID3D12CommandAllocator>>    m_command_allocators;
        ComPtr<ID3D12GraphicsCommandList>              m_graphics_cmd_list;

        // Test Pipeline
        ComPtr<ID3D12RootSignature> m_root_signature;
        ComPtr<ID3D12PipelineState> m_pipeline_state;

        // Sync objects
        uint32_t                m_frame_index {0};
        std::array<uint64_t, 2> m_fence_values = {0, 0};
        ComPtr<ID3D12Fence>     m_fence;
        HANDLE                  m_fence_event;
    };
}