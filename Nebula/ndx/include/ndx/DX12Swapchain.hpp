#pragma once

#include "core.hpp"

namespace Nebula::ndx
{
    class DX12Swapchain
    {
    public:
        NDX_DEF_PRIMARY_CTOR(DX12Swapchain, const DX12SwapchainInfo& swapchainInfo, const ComPtr<IDXGIFactory4>& pFactory, const DX12DevicePtr& pDevice);


        void present(uint32_t syncInterval = 1, uint32_t flags = 0) const;

        void setViewportScissor(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;


        const ComPtr<IDXGISwapChain3>& handle() const { return mSwapchain; }

        D3D12_CPU_DESCRIPTOR_HANDLE getRTVHandle(uint32_t i) const;

        const ComPtr<ID3D12Resource>& getRTV(uint32_t i) const;

        uint32_t getBackBufferCount() const { return mBackBufferCount; }

        uint32_t getCurrentBackBufferIndex() const;

        const Size2D& size() const { return mSize; }

    private:
        WindowPtr                           mWindow;

        ComPtr<IDXGISwapChain3>             mSwapchain;

        CD3DX12_VIEWPORT                    mViewport;
        CD3DX12_RECT                        mScissor;

        // TODO: RTV etc. abstraction
        uint32_t                            mRTVSize {0};
        ComPtr<ID3D12DescriptorHeap>        mRTVHeap;
        std::vector<ComPtr<ID3D12Resource>> mRenderTargetViews;

        Size2D                              mSize;
        const uint32_t                      mBackBufferCount;
    };
}