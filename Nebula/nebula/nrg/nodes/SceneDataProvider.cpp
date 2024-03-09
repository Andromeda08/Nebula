#include "SceneDataProvider.hpp"
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    const std::vector<ResourceSpecification> SceneDataProvider::s_resource_specifications = {
        { "Scene Data", ResourceUsage::eOutput, ResourceType::eSceneData },
    };

    SceneDataProvider::SceneDataProvider(const std::shared_ptr<ns::Scene>& scene)
    : Node("Scene Data Provider", NodeType::eSceneDataProvider), m_scene(scene)
    {
    }

    void SceneDataProvider::set_scene(const std::shared_ptr<ns::Scene>& scene)
    {
        m_scene = scene;
    }

    std::vector<ResourceClaim> SceneDataProvider::get_resource_claims()
    {
        return std::vector<ResourceClaim>(std::begin(s_resource_specifications), std::end(s_resource_specifications));
    }
}