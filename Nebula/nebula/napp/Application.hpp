#pragma once

#include <cstdint>
#include <chrono>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <napp/AppConfig.hpp>
#include <ngui/GUI.hpp>
#include <nscene/Scene.hpp>
#include <nvk/Command.hpp>
#include <nvk/Context.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Swapchain.hpp>
#include <wsi/Window.hpp>

#include <glm/glm.hpp>

namespace Nebula
{
    struct HairConstants
    {
        glm::mat4 model;
        glm::ivec4 buffer_lengths;
        uint64_t vertex_buffer;
        uint64_t strand_descriptions_buffers;
    };

    class Application
    {
    public:
        explicit Application(const std::string& config_json = "");

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

        std::shared_ptr<nvk::Descriptor>                m_descriptor;
        std::vector<std::shared_ptr<nvk::Buffer>>       m_uniform_buffers;

        std::chrono::high_resolution_clock::time_point  m_last_time;
        const AppConfig                                 m_config {};
    };
}