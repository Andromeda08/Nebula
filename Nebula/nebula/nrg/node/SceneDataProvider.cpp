#include "SceneDataProvider.hpp"
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    nrg_def_resource_requirements(SceneDataProvider, ({
        std::make_shared<Requirement>("Scene Data", ResourceUsage::eOutput, ResourceType::eSceneData)
    }));

    SceneDataProvider::SceneDataProvider()
    : Node("Scene Data Provider", NodeType::eSceneDataProvider)
    {
    }
}