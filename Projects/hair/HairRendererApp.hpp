#pragma once

#include <napp/Application.hpp>
#include <nhair/HairModel.hpp>
#include <nhair/HairRenderer.hpp>

namespace Nebula
{
    class HairRendererApp : public Application
    {
    public:
        explicit HairRendererApp(const std::string& hair_model_path, bool msaa = true);

        void update() override;

        void render(const vk::CommandBuffer &command_buffer) override;

        void render_ui() override;

        ~HairRendererApp() override = default;

    private:
        // Hair Rendering
        std::shared_ptr<nhair::HairModel>    m_hair_model;
        std::shared_ptr<nhair::HairRenderer> m_hair_renderer;
    };
}