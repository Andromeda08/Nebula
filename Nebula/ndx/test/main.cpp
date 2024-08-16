#include <DX12CommandList.hpp>
#include <DX12Device.hpp>
#include <DX12Swapchain.hpp>

#include "DX12Context.hpp"
#include "TestWindow.hpp"

using namespace Nebula::ndx;

int main()
{
    auto mockWindow = std::make_shared<test::Window>();

    DX12FactoryInfo   factoryInfo   = DX12FactoryInfo().setDebugFeatures(true);
    DX12DeviceInfo    deviceInfo    = DX12DeviceInfo();
    DX12SwapchainInfo swapchainInfo = DX12SwapchainInfo().setBackBufferCount(2).setWindow(mockWindow);

    auto contextInfo = DX12ContextInfo()
        .setFactoryInfo(factoryInfo)
        .setDeviceInfo(deviceInfo)
        .setSwapchainInfo(swapchainInfo);

    auto context = DX12Context::createDX12Context(contextInfo);

    while (!glfwWindowShouldClose(mockWindow->handle()))
    {
        glfwPollEvents();

        auto swapchain = context->swapchain();
        auto frameIndex = context->currentFrameIndex();

        auto& commandList = context->directCommandList()->beginCommandList(frameIndex);
        ComPtr<ID3D12GraphicsCommandList> graphicsCommandList;
        commandList.As(&graphicsCommandList);

        context->bindTestPipeline(graphicsCommandList);

        swapchain->setViewportScissor(graphicsCommandList);

        // Present -> RT
        auto& rtv = swapchain->getRTV(frameIndex);
        auto barrier0 = CD3DX12_RESOURCE_BARRIER::Transition(rtv.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        graphicsCommandList->ResourceBarrier(1, &barrier0);

        // Swapchain RTV
        auto rtvHandle = swapchain->getRTVHandle(frameIndex);
        graphicsCommandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

        // Clear RTV
        const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        graphicsCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        // Render things
        graphicsCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        graphicsCommandList->DrawInstanced(4, 1, 0, 0);

        // RT -> Present
        auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(rtv.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        graphicsCommandList->ResourceBarrier(1, &barrier1);

        graphicsCommandList->Close();

        std::vector<ID3D12CommandList*> pCommandLists = { graphicsCommandList.Get() };
        context->device()->queueDirect()->ExecuteCommandLists(pCommandLists.size(), pCommandLists.data());

        swapchain->present();
        context->syncNextFrame();
    }
    context->waitIdle();

    return 0;
}