#pragma once

#include "core.hpp"

namespace Nebula::ndx
{
    class DX12CommandList
    {
    public:
        NDX_DISABLE_COPY(DX12CommandList)
        NDX_DEF_PRIMARY_CTOR(DX12CommandList, const DX12CommandListInfo& info, const DX12DevicePtr& device);

        /**
         * Calling this function will reset the CommandAllocator and the requested CommandList,
         * for basic get functionality see getCommandList().
         */
        ComPtr<ID3D12CommandList>& beginCommandList(size_t frameIndex);

        ComPtr<ID3D12CommandList>& getCommandList();

    private:
        D3D12_COMMAND_LIST_TYPE                     mType;
        std::vector<ComPtr<ID3D12CommandAllocator>> mCommandAllocators;
        ComPtr<ID3D12CommandList>                   mCommandList;
        DX12DevicePtr                               mDevice;
    };
}