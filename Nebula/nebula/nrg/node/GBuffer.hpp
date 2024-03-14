#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <nrg/common/Node.hpp>
#include <nrg/common/NodeConfiguration.hpp>
#include <nrg/common/NodeTraits.hpp>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/resource/Requirement.hpp>
#include <nvk/Device.hpp>

namespace Nebula::nrg
{
    class GBuffer : public Node
    {
    public:
        explicit GBuffer(const std::shared_ptr<nvk::Device>& device)
        : Node("G-Buffer Pass", NodeType::eGBuffer)
        ,m_device(device) {}

        ~GBuffer() override = default;

        void initialize() override {}

        void execute(const vk::CommandBuffer& command_buffer) override {}

        void update() override {}

    private:
        std::shared_ptr<nvk::Device> m_device;

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