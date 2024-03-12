#include "AntiAliasing.hpp"
#include <nrg/common/NodeType.hpp>
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
#pragma region "Anti-Aliasing Node Resources"
    const std::vector<ResourceSpecification> AntiAliasing::s_resource_specifications = {
        { s_input_name, ResourceUsage::eInput, ResourceType::eImage },
        { s_output_name, ResourceUsage::eOutput, ResourceType::eImage },
    };
#pragma endregion

    AntiAliasing::AntiAliasing(const std::shared_ptr<Configuration>& configuration,
                               const std::shared_ptr<nvk::Device>& device)
    : Node("Anti-Aliasing", NodeType::eAntiAliasing), m_configuration(*configuration)
    {

    }

    void AntiAliasing::initialize()
    {
    }

    void AntiAliasing::execute(const vk::CommandBuffer& command_buffer)
    {

    }

    void AntiAliasing::update()
    {

    }

    std::vector<ResourceClaim> AntiAliasing::get_resource_claims()
    {
        return std::vector<ResourceClaim>(std::begin(s_resource_specifications), std::end(s_resource_specifications));
    }

    void AntiAliasing::Configuration::render()
    {

    }

    bool AntiAliasing::Configuration::validate()
    {
        return true;
    }
}
