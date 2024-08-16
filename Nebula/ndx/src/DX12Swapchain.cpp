#include "DX12Swapchain.hpp"
#include "DX12Device.hpp"
#include "IWindow.hpp"

namespace Nebula::ndx
{
    DX12Swapchain::DX12Swapchain(const DX12SwapchainInfo& swapchainInfo, const ComPtr<IDXGIFactory4>& pFactory, const DX12DevicePtr& pDevice)
    : mWindow(swapchainInfo.window), mBackBufferCount(swapchainInfo.backBufferCount)
    {
        mSize = mWindow->framebufferSize();

        DXGI_SWAP_CHAIN_DESC1 swapchainDesc {};
        swapchainDesc.BufferCount      = mBackBufferCount;
        swapchainDesc.Width            = mSize.width;
        swapchainDesc.Height           = mSize.height;
        swapchainDesc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchainDesc.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapchainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapchain;
        NDX_CHECK(pFactory->CreateSwapChainForHwnd(pDevice->queueDirect().Get(), mWindow->getWin32Handle(), &swapchainDesc, nullptr, nullptr, &swapchain),
                  "Failed to create DXGISwapChain");

        swapchain.As(&mSwapchain);

        D3D12_DESCRIPTOR_HEAP_DESC rtvDesc {};
        rtvDesc.NumDescriptors  = mBackBufferCount;
        rtvDesc.Type            = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvDesc.Flags           = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        NDX_CHECK(pDevice->handle()->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&mRTVHeap)),
                  "Failed to create Descriptor Heap for Swapchain");
        NDX_CHECK(mRTVHeap->SetName(L"Swapchain Descriptor Heap"), "Failed to name ID3D12DescriptorHeap");

        mRTVSize = pDevice->handle()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());
        mRenderTargetViews.resize(mBackBufferCount);
        for (size_t i = 0; i < mBackBufferCount; i++)
        {
            NDX_CHECK(mSwapchain->GetBuffer(i, IID_PPV_ARGS(&mRenderTargetViews[i])),
                      fmt::format("Failed to get Swapchain RTV #{}", i));
            pDevice->handle()->CreateRenderTargetView(mRenderTargetViews[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, mRTVSize);
        }

        mViewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(mSize.width), static_cast<float>(mSize.height));
        mScissor  = CD3DX12_RECT(0, 0, static_cast<long>(mSize.width), static_cast<long>(mSize.height));

        NDX_IF_DEBUG(fmt::println(NDX_OK(fmt::format("Created DXGISwapChain and {} RTVs", mBackBufferCount))));
    }

    std::shared_ptr<DX12Swapchain> DX12Swapchain::createDX12Swapchain(const Nebula::ndx::DX12SwapchainInfo& swapchainInfo,
                                                                      const ComPtr<IDXGIFactory4>& pFactory,
                                                                      const Nebula::ndx::DX12DevicePtr& pDevice)
    {
        return std::make_shared<DX12Swapchain>(swapchainInfo, pFactory, pDevice);
    }

    void DX12Swapchain::present(uint32_t syncInterval, uint32_t flags) const
    {
        NDX_CHECK(mSwapchain->Present(syncInterval, flags), "Present call to Swapchain failed");
    }

    void DX12Swapchain::setViewportScissor(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
    {
        commandList->RSSetViewports(1, &mViewport);
        commandList->RSSetScissorRects(1, &mScissor);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DX12Swapchain::getRTVHandle(uint32_t i) const
    {
        if (i > mRTVSize)
        {
            NDX_THROW(fmt::format("Invalid descriptor offset {} for an increment size of {}", i, mRTVSize));
        }
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(i), mRTVSize);
        return rtvHandle;
    }

    const ComPtr<ID3D12Resource>& DX12Swapchain::getRTV(uint32_t i) const
    {
        if (i > mRenderTargetViews.size())
        {
            NDX_THROW(fmt::format("Index {} out of bounds for a RTV collection of size {}", i, mRenderTargetViews.size()));
        }
        return mRenderTargetViews[i];
    }

    uint32_t DX12Swapchain::getCurrentBackBufferIndex() const
    {
        return mSwapchain->GetCurrentBackBufferIndex();
    }
}