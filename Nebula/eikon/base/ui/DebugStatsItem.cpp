#include "DebugStatsItem.hpp"
#include <imgui.h>

namespace Eikon
{
    DebugStatsItem::DebugStatsItem(const std::shared_ptr<nvk::Device>& device)
    : UIItem(), m_device(device)
    {
    }

    void DebugStatsItem::draw()
    {
        const nvk::MemoryUsage mem_usage = m_device->get_memory_usage();
        const ImGuiIO& io = ImGui::GetIO();

        ImGui::Begin("Metrics");

        ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
        ImGui::Text("Total Memory Usage: %.2f %s", mem_usage.usage, mem_usage.usage_coefficient.c_str());
        ImGui::Text("Available Memory Budget: %.2f %s", mem_usage.budget, mem_usage.budget_coefficient.c_str());

        ImGui::End();
    }
}
