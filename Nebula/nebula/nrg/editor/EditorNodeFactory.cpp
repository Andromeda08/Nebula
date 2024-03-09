#include "EditorNodeFactory.hpp"

#include <format>
#include <iostream>
#include <vector>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/nodes/SceneDataProvider.hpp>

namespace Nebula::nrg
{
    EditorNodeFactory::EditorNodeFactory(const std::shared_ptr<Context>& context)
    : m_context(context)
    {
    }

    std::shared_ptr<EditorNode> EditorNodeFactory::create(NodeType node_type)
    {
        std::vector<ResourceClaim> resource_claims = {};
        std::shared_ptr<NodeConfiguration> node_configuration = nullptr;
        using enum NodeType;
        switch (node_type)
        {
            case eSceneDataProvider:
                resource_claims = SceneDataProvider::get_resource_claims();
                break;
            default:
                throw std::runtime_error(std::format("EditorNode creation for {} node type not supported", to_string(node_type)));
        }

        return std::make_shared<EditorNode>(node_type, to_string(node_type), to_node_colors(node_type),
                                            resource_claims, node_configuration);
    }
}