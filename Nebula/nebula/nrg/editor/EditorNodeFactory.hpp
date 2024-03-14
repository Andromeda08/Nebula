#pragma once

#include <map>
#include <memory>
#include <nrg/common/Context.hpp>
#include <nrg/common/NodeTraits.hpp>
#include <nrg/editor/EditorNode.hpp>

namespace Nebula::nrg
{
    class EditorNodeFactory
    {
    public:
        EditorNodeFactory(std::map<NodeType, NodeColors>& node_colors, const std::shared_ptr<Context>& context);

        std::shared_ptr<EditorNode> create(NodeType node_type);

    private:
        std::map<NodeType, NodeColors>& m_node_colors;
        std::shared_ptr<Context>        m_context;
    };
}