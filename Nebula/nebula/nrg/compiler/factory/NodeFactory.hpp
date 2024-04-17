#pragma once

#include <memory>
#include <nrg/common/Context.hpp>
#include <nrg/common/Node.hpp>
#include <nrg/common/NodeTraits.hpp>
#include <nrg/editor/EditorNode.hpp>

namespace Nebula::nrg
{
    class NodeFactory
    {
    public:
        explicit NodeFactory(const std::shared_ptr<Context>& context) : m_context(context) {}

        std::shared_ptr<Node> create(const std::shared_ptr<EditorNode>& editor_node);

    private:
        std::shared_ptr<Context> m_context;
    };
}