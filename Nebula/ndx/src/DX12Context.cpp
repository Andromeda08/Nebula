#include "DX12Context.hpp"

#include <cstdint>
#include <DX12CommandList.hpp>
#include <DX12Shader.hpp>

#include "DX12Device.hpp"
#include "DX12Swapchain.hpp"
#include "DX12Pipeline.hpp"

namespace Nebula::ndx
{
    DX12Context::DX12Context(const DX12ContextInfo& info)
    {
        createFactory(info.factoryInfo);

        mDevice = DX12Device::createDX12Device(info.deviceInfo, mFactory.Get());
        mSwapchain = DX12Swapchain::createDX12Swapchain(info.swapchainInfo, mFactory, mDevice);

        createCommandLists();

        createFrameSynchronizationObjects();

        NDX_IF_DEBUG(createTestPipeline());
    }

    void DX12Context::waitIdle()
    {
        NDX_CHECK(mDevice->queueDirect()->Signal(mFence.Get(), mFenceValues[mFrameIndex]), "Failed to signal Fence");
        NDX_CHECK(mFence->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent), "Fence set even on completion failed");

        WaitForSingleObjectEx(mFenceEvent, INFINITE, false);
        mFenceValues[mFrameIndex]++;
    }

    void DX12Context::syncNextFrame()
    {
        const uint64_t currentFence = mFenceValues[mFrameIndex];
        NDX_CHECK(mDevice->queueDirect()->Signal(mFence.Get(), currentFence), "Failed to signal Fence");

        mFrameIndex = mSwapchain->getCurrentBackBufferIndex();

        if (mFence->GetCompletedValue() < mFenceValues[mFrameIndex])
        {
            NDX_CHECK(mFence->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent), "Fence set even on completion failed");
            WaitForSingleObjectEx(mFenceEvent, INFINITE, false);
        }

        mFenceValues[mFrameIndex] = currentFence + 1;
    }

    uint32_t DX12Context::currentFrameIndex() const
    {
        return mFrameIndex;
    }

    DX12ContextPtr DX12Context::createDX12Context(const DX12ContextInfo& info)
    {
        return std::make_shared<DX12Context>(info);
    }

    void DX12Context::createFactory(const DX12FactoryInfo& factoryInfo)
    {
        uint32_t factory_flags = 0;
        if (factoryInfo.debugFeatures)
        {
            NDX_CHECK(
                D3D12GetDebugInterface(IID_PPV_ARGS(&mDebug)),
                "Failed to initialize D3D12 Debug Interface"
            );
            NDX_IF_DEBUG(fmt::println(NDX_OK("Initialized D3D12 Debug Interface"));)

            mDebug->EnableDebugLayer();
            factory_flags = DXGI_CREATE_FACTORY_DEBUG;
        }

        NDX_CHECK(
            CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&mFactory)),
            "Failed to create DXGI Factory");

        NDX_IF_DEBUG(fmt::println(NDX_OK("Created DXGIFactory")));

        #ifdef NDX_DEBUG
        constexpr UUID D3D12ExperimentalShaderModels = {
            0x76f5573e,
            0xf13a,
            0x40f5,
            { 0xb2, 0x97, 0x81, 0xce, 0x9e, 0x18, 0x93, 0x3f }
        };

        NDX_CHECK(
            D3D12EnableExperimentalFeatures(1, &D3D12ExperimentalShaderModels, nullptr, nullptr),
            "Failed to enable experimental D3D12 features");
        #endif
    }

    void DX12Context::createCommandLists()
    {
        const auto directCommandListInfo = DX12CommandListInfo()
            .setCommandListCount(mSwapchain->getBackBufferCount())
            .setType(D3D12_COMMAND_LIST_TYPE_DIRECT);
        mDirectCommandList = DX12CommandList::createDX12CommandList(directCommandListInfo, mDevice);

        if (mDevice->queueCompute())
        {
            const auto computeCommandListInfo = DX12CommandListInfo()
                .setCommandListCount(1)
                .setType(D3D12_COMMAND_LIST_TYPE_COMPUTE);
            mComputeCommandList = DX12CommandList::createDX12CommandList(computeCommandListInfo, mDevice);
        }

        const auto copyCommandListInfo = DX12CommandListInfo()
            .setCommandListCount(1)
            .setType(D3D12_COMMAND_LIST_TYPE_COPY);
        mCopyCommandList = DX12CommandList::createDX12CommandList(copyCommandListInfo, mDevice);
    }

    void DX12Context::createFrameSynchronizationObjects()
    {
        mDevice->createFence(0, D3D12_FENCE_FLAG_NONE, mFence);
        NDX_CHECK(mFence->SetName(L"Frame Synchronization Fence"), "Failed to name ID3D12Fence");

        mFenceEvent = CreateEvent(nullptr, false, false, nullptr);

        mFenceValues.resize(mSwapchain->getBackBufferCount());
        for (auto& fenceValue : mFenceValues)
        {
            fenceValue = 0;
        }
    }

    #ifdef NDX_DEBUG
    void DX12Context::createTestPipeline()
    {
        const auto vertShader = DX12ShaderInfo().setFilePath("ndx_test.vert.cso").setShaderStage(ShaderStage::Vertex);
        const auto fragShader = DX12ShaderInfo().setFilePath("ndx_test.frag.cso").setShaderStage(ShaderStage::Fragment);

        const auto pipelineInfo = DX12PipelineInfo()
            .setCullMode(D3D12_CULL_MODE_NONE)
            .setName("NDX Test Pipeline")
            .setPipelineType(PipelineType::Graphics)
            .setRenderTargetCount(1)
            .addShader(DX12Shader::createDX12Shader(vertShader))
            .addShader(DX12Shader::createDX12Shader(fragShader));

        mTestPipeline = DX12Pipeline::createDX12Pipeline(pipelineInfo, mDevice);
    }

    void DX12Context::bindTestPipeline(const ComPtr<ID3D12GraphicsCommandList>& graphicsCommandList) const
    {
        mTestPipeline->bind(graphicsCommandList);
    }
    #endif

}