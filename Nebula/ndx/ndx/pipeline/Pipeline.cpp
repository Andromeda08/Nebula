#include "Pipeline.hpp"
#include <algorithm>
#include <d3dx12.h>
#include "Shader.hpp"
#include "../Utility.hpp"

namespace Nebula::ndx
{
    Pipeline::Pipeline(const PipelineCreateInfo& create_info, const std::shared_ptr<Device>& device)
    : m_device(device)
    {
        create_root_signature(create_info);

        auto rasterizer_state = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        rasterizer_state.CullMode = convert(create_info.cull_mode);
        rasterizer_state.FillMode = convert(create_info.fill_mode);

        auto blend_state = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

        std::vector<D3D12_INPUT_ELEMENT_DESC> input_elements;
        for (const auto& vi : create_info.m_vertex_inputs)
        {
            auto elements = vi.get_elements();
            std::ranges::for_each(elements, [&](const auto& e){ input_elements.push_back(e); });
        }

        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
        desc.pRootSignature = m_root_signature.Get();
        desc.InputLayout = { input_elements.data(), static_cast<UINT>(input_elements.size()) };
        desc.RasterizerState = rasterizer_state;
        desc.BlendState = blend_state;
        desc.SampleDesc.Count = static_cast<UINT>(create_info.sample_count);

        assign_shaders(desc, create_info.m_shaders);

        // TODO: Replace these with PipelineCreateInfo parameters
        desc.DepthStencilState.DepthEnable = true;
        desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        desc.DepthStencilState.StencilEnable = false;
        desc.SampleMask = UINT_MAX;
        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        desc.NumRenderTargets = 1;
        desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        if (auto result = m_device->handle()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pso));
            FAILED(result))
        {
            throw make_exception("Failed to create graphics pipeline: {}", create_info.name);
        }
    }

    void Pipeline::create_root_signature(const PipelineCreateInfo& create_info)
    {
        auto flags = create_info.m_vertex_inputs.empty()
                     ? D3D12_ROOT_SIGNATURE_FLAG_NONE
                     : D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        m_device->make_root_signature(&m_root_signature, flags);
    }

    void Pipeline::assign_shaders(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc,
                                  const std::vector<std::shared_ptr<Shader>>& shaders)
    {
        using enum ShaderStage;
        for (const auto& shader : shaders)
        {
            ShaderStage shader_stage = shader->get_shader_stage();
            auto blob = CD3DX12_SHADER_BYTECODE(shader->handle());
            switch (shader_stage)
            {
                case ShaderStage::eVertex:
                    desc.VS = blob;
                    break;
                case ShaderStage::eFragment:
                    desc.PS = blob;
                    break;
                default:
                    break;
            }
        }
    }

    void Pipeline::bind(const ComPtr<ID3D12GraphicsCommandList>& command_list)
    {
        command_list->SetPipelineState(m_pso.Get());
        command_list->SetGraphicsRootSignature(m_root_signature.Get());
    }
}