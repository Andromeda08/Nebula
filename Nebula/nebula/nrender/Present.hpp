#pragma once

#include <memory>
#include <vector>
#include <nscene/Scene.hpp>
#include <nvk/Barrier.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Command.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Device.hpp>
#include <nvk/Image.hpp>
#include <nvk/Swapchain.hpp>
#include <nvk/render/Framebuffer.hpp>
#include <nvk/render/Pipeline.hpp>
#include <nvk/render/RenderPass.hpp>

namespace Nebula::nrender
{
    class Present
    {
    public:
        Present(const std::shared_ptr<nvk::Device>& device,
                const std::shared_ptr<nvk::Swapchain>& swapchain,
                const std::shared_ptr<nvk::Image>& source);

        void render(uint32_t current_frame, const vk::CommandBuffer& command_buffer) const;

    private:
        std::shared_ptr<nvk::Image>       m_source;
        vk::Extent2D                      m_size;
        std::shared_ptr<nvk::Pipeline>    m_pipeline;
        std::shared_ptr<nvk::Framebuffer> m_framebuffers;
        std::shared_ptr<nvk::Descriptor>  m_descriptor;
        std::shared_ptr<nvk::RenderPass>  m_render_pass;
        std::shared_ptr<nvk::Device>      m_device;
        std::shared_ptr<nvk::Swapchain>   m_swapchain;
    };
}