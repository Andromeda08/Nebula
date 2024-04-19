#pragma once

#include <memory>
#include <vector>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nscene/Scene.hpp>
#include <nvk/Barrier.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Command.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Device.hpp>
#include <nvk/Image.hpp>
#include <nvk/Swapchain.hpp>
#include <nvk/render/Pipeline.hpp>

namespace Nebula::nrender
{
    struct RaytracerPushConstant
    {
        glm::vec4 light_position  { 1.0f, 10.0f, 1.0f, 0.0f };
        glm::vec4 light_intensity { 1.0f, 1.0f, 1.0f, 1.0f };
        float     spot_cutoff {0.0f};
        float     spot_outer_cutoff {0.0f};
        float     _pad11 {}, _pad12 {};
        int32_t   light_type {0};
        int32_t   _pad13 {}, _pad14 {}, _pad15 {};
    };

    class Raytracer
    {
    public:
        Raytracer(const std::shared_ptr<nvk::Device>& device,
                  const std::shared_ptr<nvk::CommandPool>& command_pool,
                  const std::shared_ptr<nvk::Swapchain>& swapchain,
                  const std::shared_ptr<ns::Scene>& scene);

        void render(uint32_t current_frame, const vk::CommandBuffer& command_buffer) const;

        void render_ui();

        const std::shared_ptr<nvk::Image>& target() const { return m_target; }

    private:
        RaytracerPushConstant                     m_push_constant;
        vk::Extent2D                              m_size;
        std::shared_ptr<ns::Scene>                m_scene;
        std::shared_ptr<nvk::Image>               m_target;
        std::shared_ptr<nvk::Pipeline>            m_pipeline;
        std::shared_ptr<nvk::Descriptor>          m_descriptor;
        std::vector<std::shared_ptr<nvk::Buffer>> m_uniform_buffer;
        std::shared_ptr<nvk::Device>              m_device;
        std::shared_ptr<nvk::Swapchain>           m_swapchain;
    };
}