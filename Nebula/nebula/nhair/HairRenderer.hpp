#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <nhair/HairModel.hpp>
#include <nscene/Camera.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Device.hpp>
#include <nvk/Image.hpp>
#include <nvk/Swapchain.hpp>
#include <nvk/render/Framebuffer.hpp>
#include <nvk/render/Pipeline.hpp>
#include <nvk/render/RenderPass.hpp>

namespace Nebula::nhair
{
    struct HairConstants
    {
        glm::mat4 model;
        glm::ivec4 buffer_lengths;
        uint64_t vertex_buffer;
        uint64_t strand_descriptions_buffers;
    };

    class HairRenderer
    {
    public:
        explicit HairRenderer(const std::shared_ptr<nvk::Device>& device,
                              const std::shared_ptr<nvk::Swapchain>& swapchain);

        void render(uint32_t current_frame,
                    const HairModel& hair_model,
                    const ns::CameraData& camera_data,
                    const vk::CommandBuffer& command_buffer) const;

    private:
        vk::Extent2D                              m_render_res;
        std::shared_ptr<nvk::Descriptor>          m_descriptor;
        std::array<vk::ClearValue, 2>             m_clear_values;
        std::shared_ptr<nvk::Framebuffer>         m_framebuffers;
        std::shared_ptr<nvk::Image>               m_depth;
        std::shared_ptr<nvk::Pipeline>            m_pipeline;
        vk::RenderPass                            m_render_pass;
        std::vector<std::shared_ptr<nvk::Buffer>> m_uniform_buffers;
    };
}