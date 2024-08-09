 #pragma once

#include <napp/Application.hpp>
#include <nrender/DebugRenderer.hpp>
#include <nrender/Present.hpp>
#include <nrender/Raytracer.hpp>
#include "render/basic/Texture.hpp"

namespace Nebula
{
    class RenderGraphApp : public Application
    {
    public:
        RenderGraphApp();

        void render(const vk::CommandBuffer &command_buffer) override;

        ~RenderGraphApp() override = default;
    };
}