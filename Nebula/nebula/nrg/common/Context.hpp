#pragma once

#include <memory>
#include <vector>
#include <ncommon/Size2D.hpp>
#include <nscene/Scene.hpp>
#include <nrg/common/RenderPath.hpp>
#include <nvk/Command.hpp>
#include <nvk/Device.hpp>
#include <nvk/Swapchain.hpp>

namespace Nebula::nrg
{
    struct Context
    {
        Context(const std::vector<std::shared_ptr<ns::Scene>>& scenes,
                const std::shared_ptr<nvk::Device>& device,
                const std::shared_ptr<nvk::CommandPool>& command_pool,
                const std::shared_ptr<nvk::Swapchain>& swapchain)
        : m_scene_list(scenes), m_device(device), m_command_pool(command_pool), m_swapchain(swapchain)
        {
            auto extent = swapchain->extent();
            m_render_resolution = { extent.width, extent.height };
            m_target_resolution = { extent.width, extent.height };
            m_frames = swapchain->image_count();
            m_current_frame = 0;
        }

        const std::shared_ptr<ns::Scene>& get_selected_scene() const
        {
            return m_scene_list[m_selected_scene];
        }

        // Available Scenes -------------------------------------------------
        const std::vector<std::shared_ptr<ns::Scene>>& m_scene_list;
        int32_t                                        m_selected_scene {0};

        // Last compiled executable graph -----------------------------------
        std::shared_ptr<RenderPath>                     m_render_path;

        // Rendering Context ------------------------------------------------
        Size2D                                          m_render_resolution;
        Size2D                                          m_target_resolution;
        std::shared_ptr<nvk::Device>                    m_device;
        std::shared_ptr<nvk::CommandPool>               m_command_pool;
        std::shared_ptr<nvk::Swapchain>                 m_swapchain;
        uint32_t                                        m_frames;
        uint32_t                                        m_current_frame;
    };
}