 #pragma once

#include <napp/Application.hpp>

namespace Nebula
{
    class VkPlayground : public Application
    {
    public:
        VkPlayground();

        void render(const vk::CommandBuffer &command_buffer) override;

        ~VkPlayground() override = default;
    };
}