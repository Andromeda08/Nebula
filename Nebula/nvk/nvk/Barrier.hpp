#pragma once

#include <vulkan/vulkan.hpp>
#include "Utility.hpp"

namespace Nebula::nvk
{
    class Barrier
    {
    public:
        virtual ~Barrier() = default;
    };

    class BarrierBatch
    {
    public:
    };

    class ImageBarrier : public Barrier
    {

    };
}