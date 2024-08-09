#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <nrg/common/Context.hpp>
#include <nrg/common/Node.hpp>
#include <nrg/common/NodeConfiguration.hpp>
#include <nrg/common/NodeTraits.hpp>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/resource/Requirement.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Device.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Swapchain.hpp>
#include <nvk/render/Framebuffer.hpp>
#include <nvk/render/Pipeline.hpp>
#include <nvk/render/RenderPass.hpp>

namespace Nebula::nrg
{
    class Present : public Node
    {
    public:
        explicit Present(const std::shared_ptr<Context>& context)
        : Node("Present", NodeType::ePresent)
        , m_context(context)
        , m_device(context->m_device)
        , m_current_frame(context->m_current_frame)
        , m_swapchain(context->m_swapchain)
        {
        }

        ~Present() override = default;

        void initialize() override;

        void execute(const vk::CommandBuffer& command_buffer) override;

        void update() override;

    private:
        std::shared_ptr<Context>                    m_context;
        std::shared_ptr<nvk::Device>                m_device;
        std::shared_ptr<nvk::Swapchain>             m_swapchain;

        uint32_t&                                   m_current_frame;
        std::shared_ptr<nvk::RenderPass>            m_render_pass;
        std::shared_ptr<nvk::Pipeline>              m_pipeline;
        std::shared_ptr<nvk::Framebuffer>           m_framebuffers;
        std::shared_ptr<nvk::Descriptor>            m_descriptor;
        std::vector<std::shared_ptr<nvk::Buffer>>   m_uniform_buffer;

        static constexpr const char* s_input = "Present";

        nrg_decl_resource_requirements();
        nrg_def_get_resource_claims();
    };
}