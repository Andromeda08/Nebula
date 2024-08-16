#include "DX12CommandList.hpp"
#include "DX12Device.hpp"

namespace Nebula::ndx
{
    DX12CommandList::DX12CommandList(const DX12CommandListInfo& info, const DX12DevicePtr& device)
    : mDevice(device), mType(info.type)
    {
        if (info.commandListCount == 0)
        {
            NDX_IF_DEBUG(fmt::println(NDX_WARN("DX12CommandList created with commandListCount set to 0")));
        }

        mCommandAllocators.resize(info.commandListCount);
        for (auto& commandAllocator : mCommandAllocators)
        {
            mDevice->createCommandAllocator(mType, commandAllocator);
        }

        mDevice->createCommandList(0, mType, mCommandAllocators[0].Get(), nullptr, mCommandList);
        if (mType == D3D12_COMMAND_LIST_TYPE_DIRECT)
        {
            ComPtr<ID3D12GraphicsCommandList> graphicsCommandList;
            NDX_CHECK_IF_DEBUG(mCommandList.As(&graphicsCommandList), "Failed to cast CommandList to GraphicsCommandList");
            NDX_CHECK(graphicsCommandList->Close(), "Failed to close GraphicsCommandList");
        }

        NDX_IF_DEBUG(fmt::println(NDX_OK(
            fmt::format("Created {} DX12CommandList with {} command allocators", fmt::styled(to_string(info.type), fmt::fg(to_color(info.type))), mCommandAllocators.size())
        )));
    }

    std::shared_ptr<DX12CommandList> DX12CommandList::createDX12CommandList(const DX12CommandListInfo& info, const DX12DevicePtr& device)
    {
        return std::make_shared<DX12CommandList>(info, device);
    }

    ComPtr<ID3D12CommandList>& DX12CommandList::beginCommandList(const size_t frameIndex)
    {
        if (frameIndex > mCommandAllocators.size())
        {
            NDX_THROW(fmt::format("Index {} is out of bounds for container of size {}", frameIndex, mCommandAllocators.size()));
        }

        NDX_CHECK(mCommandAllocators[frameIndex]->Reset(), "Failed to reset CommandAllocator");

        if (mType == D3D12_COMMAND_LIST_TYPE_DIRECT)
        {
            ComPtr<ID3D12GraphicsCommandList> graphicsCommandList;
            NDX_CHECK_IF_DEBUG(mCommandList.As(&graphicsCommandList), "Failed to cast CommandList to GraphicsCommandList");
            NDX_CHECK(graphicsCommandList->Reset(mCommandAllocators[frameIndex].Get(), nullptr), "Failed to reset GraphicsCommandList");
            return mCommandList;
        }

        NDX_THROW("Unexpected error");
    }

    ComPtr<ID3D12CommandList>& DX12CommandList::getCommandList()
    {
        return mCommandList;
    }
}
