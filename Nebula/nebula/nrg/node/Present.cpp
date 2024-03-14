#include "Present.hpp"
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    nrg_def_resource_requirements(Present, ({
        std::make_shared<ImageRequirement>(s_input, ResourceUsage::eInput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
    }));
}