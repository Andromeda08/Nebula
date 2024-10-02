#pragma once

#include <nvk/Device.hpp>
#include "base/UIItem.hpp"

namespace Eikon
{
    using namespace Nebula;
    class DebugStatsItem final : public UIItem
    {
    public:
        explicit DebugStatsItem(const std::shared_ptr<nvk::Device>& device);

        void draw() override;

        void update() override {}

        ~DebugStatsItem() override = default;

    private:
        std::shared_ptr<nvk::Device> m_device;
    };
}