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
    class Present : public Node
    {
    public:
        explicit Present(const std::shared_ptr<nvk::Device>& device)
        : Node("Present", NodeType::ePresent)
        , m_device(device) {}

        ~Present() override = default;

        void initialize() override {}

        void execute(const vk::CommandBuffer& command_buffer) override {}

        void update() override {}

    private:
        std::shared_ptr<nvk::Device> m_device;

        static constexpr const char* s_input = "Present";

        nrg_decl_resource_requirements();
        nrg_def_get_resource_claims();
    };
}