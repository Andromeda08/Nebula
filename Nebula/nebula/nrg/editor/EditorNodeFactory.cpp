#include "EditorNodeFactory.hpp"

#include <format>
#include <iostream>
#include <vector>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/nodes/Nodes.hpp>

namespace Nebula::nrg
{
    EditorNodeFactory::EditorNodeFactory(std::map<NodeType, NodeColors>& node_colors, const std::shared_ptr<Context>& context)
    : m_node_colors(node_colors), m_context(context)
    {
    }

    std::shared_ptr<EditorNode> EditorNodeFactory::create(NodeType node_type)
    {
        std::vector<ResourceClaim> resource_claims = {};
        std::shared_ptr<NodeConfiguration> node_configuration = nullptr;
        NodeColors node_colors = (m_node_colors.contains(node_type)) ? m_node_colors[node_type] : NodeColors();

        using enum NodeType;
        switch (node_type)
        {
            case eAntiAliasing:
                resource_claims = AntiAliasing::get_resource_claims();
                node_configuration = std::make_shared<AntiAliasing::Configuration>();
                break;
            case eSceneDataProvider:
                resource_claims = SceneDataProvider::get_resource_claims();
                break;
            default:
                throw std::runtime_error(std::format("EditorNode creation for {} node type not supported", to_string(node_type)));
        }

        return std::make_shared<EditorNode>(node_type, to_string(node_type), node_colors,
                                            resource_claims, node_configuration);
    }
}