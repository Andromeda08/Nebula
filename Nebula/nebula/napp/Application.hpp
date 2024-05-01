#pragma once

#include <cstdint>
#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <napp/AppConfig.hpp>
#include <ngui/GUI.hpp>
#include <nhair/HairModel.hpp>
#include <nhair/HairRenderer.hpp>
#include <nrender/DebugRenderer.hpp>
#include <nrender/Present.hpp>
#include <nrender/Raytracer.hpp>
#include <nrender/SBuffer.hpp>
#include <nrg/common/Context.hpp>
#include <nrg/editor/GraphEditor.hpp>
#include <nscene/Scene.hpp>
#include <nvk/Command.hpp>
#include <nvk/Context.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Swapchain.hpp>
#include <wsi/Window.hpp>

namespace Nebula
{
    class Application
    {
    public:
        explicit Application(std::optional<std::string> hair_file = std::nullopt);

        void run();

        void loop();

    public:
        static constexpr uint32_t s_max_frames_in_flight {2};
        static           uint32_t s_current_frame;
        static           Size2D   s_extent;

    private:
        void init_render_context();

        float delta_time();

        std::shared_ptr<wsi::Window>                    m_window;
        std::shared_ptr<nvk::Context>                   m_context;
        std::shared_ptr<nvk::CommandRing>               m_command_ring;
        std::shared_ptr<nvk::Swapchain>                 m_swapchain;
        std::shared_ptr<ngui::GUI>                      m_gui;
        std::vector<std::shared_ptr<ns::Scene>>         m_scenes;
        std::shared_ptr<ns::Scene>                      m_active_scene;

        // Render Graph -------------------------------------------------------
        std::shared_ptr<nrg::Context>                   m_rg_context;
        std::shared_ptr<nrg::GraphEditor>               m_rg_editor;

        // Hair Renderer ------------------------------------------------------
        std::shared_ptr<nhair::HairModel>               m_hair_model;
        std::shared_ptr<nhair::HairRenderer>            m_hair_renderer;

        // Raytracer ----------------------------------------------------------
        std::shared_ptr<nrender::Raytracer>             m_ray_tracer;
        std::shared_ptr<nrender::Present>               m_present;
        std::shared_ptr<nrender::DebugRender>           m_light_debug;

        // S-Buffer Test ------------------------------------------------------
        std::shared_ptr<nrender::SBuffer>               m_sbuffer;

        std::chrono::high_resolution_clock::time_point  m_last_time;
        const AppConfig                                 m_config {};
    };
}