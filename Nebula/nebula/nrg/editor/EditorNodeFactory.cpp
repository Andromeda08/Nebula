#include "EditorNodeFactory.hpp"

#include <fmt/format.h>
#include <iostream>
#include <vector>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/node/Nodes.hpp>

#define nrg_case_RC(Enum, Type)                                     \
case Enum:                                                          \
    resource_claims = Type::get_resource_claims();                  \
    break;

#define nrg_case_RC_NC(Enum, Type)                                  \
case Enum:                                                          \
    resource_claims = Type::get_resource_claims();                  \
    node_configuration = std::make_shared<Type::Configuration>();   \
    break;

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
            nrg_case_RC_NC(eAmbientOcclusion, AmbientOcclusion);
            nrg_case_RC_NC(eAntiAliasing, AntiAliasing);
            nrg_case_RC_NC(eDeferredLighting, DeferredLighting);
            nrg_case_RC(eGBuffer, GBuffer);
            nrg_case_RC(ePresent, Present);
            nrg_case_RC(eSceneDataProvider, SceneDataProvider);
            default:
                throw std::runtime_error(fmt::format("EditorNode creation for {} node type not supported", to_string(node_type)));
        }

        return std::make_shared<EditorNode>(node_type, to_string(node_type), node_colors,
                                            resource_claims, node_configuration);
    }
}