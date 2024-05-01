#pragma once

#include <cstdint>
#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <napp/AppConfig.hpp>
#include <ngui/GUI.hpp>
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
    struct AppParameters
    {
        bool ray_tracing {false};
        bool mesh_shaders {false};
    };

    class Application
    {
    public:
        explicit Application(const std::optional<AppParameters>& params = std::nullopt);

        void run();

        virtual ~Application() = default;

    protected:
        float delta_time();

        virtual void update() {}

        virtual void render(const vk::CommandBuffer& command_buffer) {}

        virtual void render_ui() {}

    private:
        void init_render_context();

        void loop();

    public:
        static constexpr uint32_t s_max_frames_in_flight {2};
        static           uint32_t s_current_frame;

    protected:
        std::shared_ptr<wsi::Window>            m_window;
        std::shared_ptr<nvk::Context>           m_context;
        std::shared_ptr<nvk::CommandRing>       m_command_ring;
        std::shared_ptr<nvk::Swapchain>         m_swapchain;
        std::shared_ptr<ngui::GUI>              m_gui;
        std::vector<std::shared_ptr<ns::Scene>> m_scenes;
        std::shared_ptr<ns::Scene>              m_active_scene;

        // Render Graph -------------------------------------------------------
        std::shared_ptr<nrg::Context>           m_rg_context;
        std::shared_ptr<nrg::GraphEditor>       m_rg_editor;

        const AppConfig                         m_config {};
        AppParameters                           m_params {};

    private:
        std::chrono::high_resolution_clock::time_point  m_last_time;
    };
}