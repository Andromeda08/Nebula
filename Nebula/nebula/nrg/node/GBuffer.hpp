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
#include <nvk/render/Framebuffer.hpp>
#include <nvk/render/Pipeline.hpp>
#include <nvk/render/RenderPass.hpp>

namespace Nebula::nrg
{
    class GBuffer : public Node
    {
    public:
        explicit GBuffer(const std::shared_ptr<Context>& context)
        : Node("G-Buffer Pass", NodeType::eGBuffer)
        , m_context(context)
        , m_device(context->m_device)
        , m_current_frame(context->m_current_frame) {}

        ~GBuffer() override = default;

        void initialize() override;

        void execute(const vk::CommandBuffer& command_buffer) override;

        void update() override;

    private:
        std::shared_ptr<Context>                    m_context;
        std::shared_ptr<nvk::Device>                m_device;

        uint32_t&                                   m_current_frame;
        std::shared_ptr<nvk::RenderPass>            m_render_pass;
        std::shared_ptr<nvk::Pipeline>              m_pipeline;
        std::shared_ptr<nvk::Framebuffer>           m_framebuffers;
        std::shared_ptr<nvk::Descriptor>            m_descriptor;
        std::vector<std::shared_ptr<nvk::Buffer>>   m_uniform_buffer;

        static constexpr const char* s_scene_data = "Scene Data";
        static constexpr const char* s_position   = "Position Buffer";
        static constexpr const char* s_normal     = "Normal Buffer";
        static constexpr const char* s_albedo     = "Albedo Buffer";
        static constexpr const char* s_depth      = "Depth Buffer";
        static constexpr const char* s_motion_vec = "Motion Vectors";

        nrg_decl_resource_requirements();
        nrg_def_get_resource_claims();
    };
}