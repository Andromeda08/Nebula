#include "SceneDataProvider.hpp"
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    nrg_def_resource_requirements(SceneDataProvider, ({
        std::make_shared<Requirement>("Scene Data", ResourceUsage::eOutput, ResourceType::eSceneData)
    }));

    SceneDataProvider::SceneDataProvider(const std::shared_ptr<ns::Scene>& scene)
    : Node("Scene Data Provider", NodeType::eSceneDataProvider), m_scene(scene)
    {
    }
}