#include "GBuffer.hpp"
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    nrg_def_resource_requirements(GBuffer, ({
        std::make_shared<Requirement>(s_scene_data, ResourceUsage::eInput, ResourceType::eSceneData),
        std::make_shared<ImageRequirement>(s_position, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_normal, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_albedo, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_depth, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eD32Sfloat),
        std::make_shared<ImageRequirement>(s_motion_vec, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
    }));
}