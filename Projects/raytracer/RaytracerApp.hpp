 #pragma once

#include <napp/Application.hpp>
#include <nrender/DebugRenderer.hpp>
#include <nrender/Present.hpp>
#include <nrender/Raytracer.hpp>

namespace Nebula
{
    class RaytracerApp : public Application
    {
    public:
        RaytracerApp();

        void update() override;

        void render(const vk::CommandBuffer &command_buffer) override;

        void render_ui() override;

        ~RaytracerApp() override = default;

    private:
        std::shared_ptr<nrender::Raytracer>   m_raytracer;
        std::shared_ptr<nrender::Present>     m_present;
        std::shared_ptr<nrender::DebugRender> m_light_debug;

        bool m_render_debug_lights {true};
    };
}