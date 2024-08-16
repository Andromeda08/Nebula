#pragma once

#include "core.hpp"

namespace Nebula::ndx
{
    struct DX12ContextFeatures
    {
        // General
        bool     debugEnabled {false};
        uint32_t backBufferCount {2};

        // Device Features and Capabilities
    };

    class DX12Context
    {
    public:
        NDX_DISABLE_COPY(DX12Context);
        explicit NDX_DEF_PRIMARY_CTOR(DX12Context, const DX12ContextInfo& info);

        void waitIdle();

        void syncNextFrame();

        uint32_t currentFrameIndex() const;

        #ifdef NDX_DEBUG
        void bindTestPipeline(const ComPtr<ID3D12GraphicsCommandList>& graphicsCommandList) const;
        #endif

        const DX12DevicePtr&    device()    const { return mDevice;    }
        const DX12SwapchainPtr& swapchain() const { return mSwapchain; }

        const DX12CommandListPtr& computeCommandList() const { return mComputeCommandList; }
        const DX12CommandListPtr& copyCommandList()    const { return mCopyCommandList;    }
        const DX12CommandListPtr& directCommandList()  const { return mDirectCommandList;  }

    private:
        void createFactory(const DX12FactoryInfo& factoryInfo);
        void createCommandLists();
        void createFrameSynchronizationObjects();

        #ifdef NDX_DEBUG
        void createTestPipeline();
        #endif;

        // Track context capabilities and features
        DX12ContextFeatures     mFeatures {};

        // Owned GPU Objects
        DX12DevicePtr           mDevice;
        DX12SwapchainPtr        mSwapchain;

        DX12CommandListPtr      mComputeCommandList;
        DX12CommandListPtr      mCopyCommandList;
        DX12CommandListPtr      mDirectCommandList;

        // Frame synchronization
        size_t                  mFrameIndex {0};
        std::vector<uint64_t>   mFenceValues;
        ComPtr<ID3D12Fence>     mFence;
        HANDLE                  mFenceEvent {nullptr};

        ComPtr<IDXGIFactory4>   mFactory;
        ComPtr<ID3D12Debug>     mDebug;

        // Test Pipeline
        #ifdef NDX_DEBUG
        DX12PipelinePtr mTestPipeline;
        #endif
    };
}