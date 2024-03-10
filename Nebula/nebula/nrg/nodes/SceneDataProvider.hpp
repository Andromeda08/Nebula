#pragma once

#include <memory>
#include <nscene/Scene.hpp>
#include <nrg/common/INode.hpp>
#include <nrg/common/ResourceClaim.hpp>

namespace Nebula::nrg
{
    struct ResourceClaim;

    class SceneDataProvider final : public Node
    {
    public:
        explicit SceneDataProvider(const std::shared_ptr<ns::Scene>& scene);

        ~SceneDataProvider() override = default;

        void set_scene(const std::shared_ptr<ns::Scene>& scene);

        static std::vector<ResourceClaim> get_resource_claims();

        DEF_RESOURCE_SPECIFICATIONS()
    private:
        std::shared_ptr<ns::Scene> m_scene;
    };
}