#include "DX12Pipeline.hpp"
#include "DX12Device.hpp"
#include "DX12Shader.hpp"

namespace Nebula::ndx
{
    DX12Pipeline::DX12Pipeline(const DX12PipelineInfo& pipelineInfo, const DX12DevicePtr& device)
    : mDevice(device), mType(pipelineInfo.pipelineType), mName(pipelineInfo.name)
    {
        const auto rootSignatureFlags = getRootSignatureFlags(pipelineInfo.inputElements.empty());
        mDevice->makeRootSignature(&mRootSignature, rootSignatureFlags);

        auto rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        rasterizerState.CullMode = pipelineInfo.cullMode;
        rasterizerState.FillMode = pipelineInfo.fillMode;

        auto blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

        auto depthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        depthStencilState.DepthEnable = true;
        depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        depthStencilState.StencilEnable = false;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = mRootSignature.Get();
        psoDesc.InputLayout = { pipelineInfo.inputElements.data(), static_cast<UINT>(pipelineInfo.inputElements.size()) };
        psoDesc.RasterizerState = rasterizerState;
        psoDesc.BlendState = blendState;
        psoDesc.SampleDesc.Count = static_cast<UINT>(pipelineInfo.sampleCount);

        assignPipelineStateDescShaders(psoDesc, pipelineInfo.shaders);

        psoDesc.DepthStencilState = depthStencilState;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = pipelineInfo.renderTargetCount;

        // TODO: Get from DX12PipelineInfo
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        mDevice->createGraphicsPipelineState(psoDesc, mPSO);

        NDX_IF_DEBUG(fmt::println(NDX_OK(fmt::format("Created {} Pipeline ({})", fmt::styled(toString(mType), fmt::fg(toColor(mType))), mName))));
    }

    std::shared_ptr<DX12Pipeline> DX12Pipeline::createDX12Pipeline(const DX12PipelineInfo& pipelineInfo, const DX12DevicePtr& device)
    {
        return std::make_shared<DX12Pipeline>(pipelineInfo, device);
    }

    void DX12Pipeline::bind(const ComPtr<ID3D12GraphicsCommandList>& graphicsCommandList) const
    {
        graphicsCommandList->SetPipelineState(mPSO.Get());
        graphicsCommandList->SetGraphicsRootSignature(mRootSignature.Get());
    }

    D3D12_ROOT_SIGNATURE_FLAGS DX12Pipeline::getRootSignatureFlags(const bool hasVertexInputs)
    {
        return hasVertexInputs
            ? D3D12_ROOT_SIGNATURE_FLAG_NONE
            : D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    }

    void DX12Pipeline::assignPipelineStateDescShaders(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const std::vector<DX12ShaderPtr>& shaders)
    {
        using enum ShaderStage;
        for (const auto& shader : shaders)
        {
            const auto shaderStage = shader->shaderStage();
            const auto blob = CD3DX12_SHADER_BYTECODE(shader->handle().Get());
            switch (shaderStage)
            {
                case Vertex: {
                    desc.VS = blob;
                    break;
                }
                case Fragment: {
                    desc.PS = blob;
                    break;
                }
                default: {
                    NDX_IF_DEBUG(fmt::println(NDX_WARN(
                        fmt::format("Assigning {} shaders is not implemented yet", fmt::styled(toString(shaderStage), fmt::fg(toColor(shaderStage))))
                    )));
                    break;
                }
            }
        }
    }
}
