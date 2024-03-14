#pragma once

#include <memory>
#include <nscene/Scene.hpp>
#include <nrg/common/Node.hpp>
#include <nrg/common/ResourceClaim.hpp>

namespace Nebula::nrg
{
    struct ResourceClaim;

    class SceneDataProvider final : public Node
    {
    public:
        explicit SceneDataProvider(const std::shared_ptr<ns::Scene>& scene);

        ~SceneDataProvider() override = default;

    private:
        std::shared_ptr<ns::Scene> m_scene;

        nrg_decl_resource_requirements();
        nrg_def_get_resource_claims();
    };
}