#include "Context.hpp"
#include "Utility.hpp"
#include <d3dx12.h>
#include <d3dcompiler.h>
#include "pipeline/Shader.hpp"

namespace Nebula::ndx
{
    Context::Context(const ContextCreateInfo& create_info)
    {
        pInfo("Initializing {} context",green("DirectX 12"));
        m_instance = std::make_shared<Instance>(true);
        m_device = std::make_shared<Device>(*m_instance);

        SwapchainCreateInfo swapchain_create_info = {
            .frame_count = create_info.frame_count,
            .window = create_info.window,
            .instance = m_instance,
            .device = m_device,
        };
        m_swapchain = std::make_shared<Swapchain>(swapchain_create_info);

        // D3D12 CommandAllocator and Lists
        #pragma region
        m_command_allocators.resize(create_info.frame_count);
        for (uint32_t i = 0; i < m_command_allocators.size(); i++)
        {
            if (auto result = m_device->handle()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_command_allocators[i]));
                FAILED(result))
            {
                throw make_exception("Failed to create {}, type: Direct", red("D3D12CommandAllocator"));
            }
        }

        if (auto result = m_device->handle()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_command_allocators[0].Get(), nullptr, IID_PPV_ARGS(&m_graphics_cmd_list));
            FAILED(result))
        {
            throw make_exception("Failed to create {}", red("D3D12GraphicsCommandList"));
        }
        m_graphics_cmd_list->Close();

        pInfo("Created {}", green("D3D12GraphicsCommandList"));
        #pragma endregion

        // Sync objects
        #pragma region
        m_device->handle()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
        m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fence_event == nullptr) {}
        #pragma endregion

        // Test Pipeline
        #pragma region
        m_device->make_root_signature(&m_root_signature, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ShaderCreateInfo vs_ci { ShaderStage::eVertex, "dx12_test.vert.cso" };
        auto vertex_shader = std::make_shared<Shader>(vs_ci, m_device);

        ShaderCreateInfo fs_ci { ShaderStage::eFragment, "dx12_test.frag.cso" };
        auto pixel_shader = std::make_shared<Shader>(fs_ci, m_device);

        auto rasterizer_state = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        rasterizer_state.CullMode = D3D12_CULL_MODE_NONE;

        auto blend_desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
        pso_desc.InputLayout = {};
        pso_desc.pRootSignature = m_root_signature.Get();
        pso_desc.VS = CD3DX12_SHADER_BYTECODE(vertex_shader->handle());
        pso_desc.PS = CD3DX12_SHADER_BYTECODE(pixel_shader->handle());
        pso_desc.RasterizerState = rasterizer_state;
        pso_desc.BlendState = blend_desc;
        pso_desc.DepthStencilState.DepthEnable = true;
        pso_desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        pso_desc.DepthStencilState.StencilEnable = false;
        pso_desc.SampleMask = UINT_MAX;
        pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pso_desc.NumRenderTargets = 1;
        pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        pso_desc.SampleDesc.Count = 1;

        if (auto result = m_device->handle()->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&m_pipeline_state));
            FAILED(result))
        {
            throw make_exception("Failed to create graphics pipeline");
        }

        #pragma endregion

        auto pci = PipelineCreateInfo()
            .add_shader(vertex_shader)
            .add_shader(pixel_shader)
            .set_cull_mode(CullMode::eNone)
            .set_name("Test Pipeline")
            .set_target_count(1)
            .set_type(PipelineType::eGraphics);
        m_pipeline = std::make_shared<Pipeline>(pci, m_device);
    }

    void Context::wait_idle()
    {
        m_device->qGraphics()->Signal(m_fence.Get(), m_fence_values[m_frame_index]);
        m_fence->SetEventOnCompletion(m_fence_values[m_frame_index], m_fence_event);
        WaitForSingleObjectEx(m_fence_event, INFINITE, false);
        m_fence_values[m_frame_index]++;
    }

    void Context::on_render()
    {
        auto& command_list = m_graphics_cmd_list;

        m_command_allocators[m_frame_index]->Reset();
        command_list->Reset(m_command_allocators[m_frame_index].Get(), nullptr);

//        command_list->SetPipelineState(m_pipeline_state.Get());
//        command_list->SetGraphicsRootSignature(m_root_signature.Get());
        m_pipeline->bind(command_list);
        m_swapchain->set_viewport_scissor(command_list);

        // Present -> RT
        auto& rtv = m_swapchain->get_rtv(m_frame_index);
        auto barrier0 = CD3DX12_RESOURCE_BARRIER::Transition(rtv.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        command_list->ResourceBarrier(1, &barrier0);

        // Swapchain RTV
        auto rtv_handle = m_swapchain->get_rtv_handle(m_frame_index);
        command_list->OMSetRenderTargets(1, &rtv_handle, false, nullptr);

        // Clear RTV
        const float clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        command_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);

        // Render things
        command_list->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        command_list->DrawInstanced(4, 1, 0, 0);

        // RT -> Present
        auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(rtv.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        command_list->ResourceBarrier(1, &barrier1);

        command_list->Close();

        std::vector<ID3D12CommandList*> pp_command_lists = { command_list.Get() };
        m_device->qGraphics()->ExecuteCommandLists(pp_command_lists.size(), pp_command_lists.data());

        m_swapchain->present();

        move_to_next_frame();
    }

    void Context::move_to_next_frame()
    {
        const uint64_t current_fence = m_fence_values[m_frame_index];
        m_device->qGraphics()->Signal(m_fence.Get(), current_fence);
        m_frame_index = m_swapchain->get_current_back_buffer_index();

        if (m_fence->GetCompletedValue() < m_fence_values[m_frame_index])
        {
            m_fence->SetEventOnCompletion(m_fence_values[m_frame_index], m_fence_event);
            WaitForSingleObjectEx(m_fence_event, INFINITE, false);
        }

        m_fence_values[m_frame_index] = current_fence + 1;
    }

}