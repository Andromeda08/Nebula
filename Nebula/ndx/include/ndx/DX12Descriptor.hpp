#pragma once

#include "core.hpp"

namespace Nebula::ndx
{
    class DX12Descriptor
    {
    public:

    private:
        std::vector<D3D12_DESCRIPTOR_RANGE1> mDescriptorRanges;
        std::vector<D3D12_ROOT_PARAMETER1>   mRootParameters;
        D3D12_VERSIONED_ROOT_SIGNATURE_DESC  mRootSigDesc {};
    };
}