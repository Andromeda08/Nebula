#include "AntiAliasing.hpp"
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    nrg_def_resource_requirements(AntiAliasing, ({
        std::make_shared<ImageRequirement>(s_input_name, ResourceUsage::eInput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_output_name, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat)
    }));

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

    void AntiAliasing::Configuration::render()
    {

    }

    bool AntiAliasing::Configuration::validate()
    {
        return true;
    }
}
