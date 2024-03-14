#pragma once

#include <memory>
#include <nrg/common/Context.hpp>
#include <nrg/compiler/CompilerResult.hpp>
#include <nrg/compiler/NodeFactory.hpp>
#include <nrg/compiler/ResourceFactory.hpp>
#include <nrg/editor/EditorNode.hpp>
#include <nrg/editor/Graph.hpp>

namespace Nebula::nrg
{
    class CompilerStrategy
    {
        using NodePtr = std::shared_ptr<EditorNode>;
    public:
        explicit CompilerStrategy(const std::shared_ptr<Context>& context);

        virtual CompilerResult compile(const Graph& graph) = 0;

        virtual ~CompilerStrategy() = default;

    protected:
        // Common compiler steps ----------------------------------------------
        static std::vector<NodePtr> filter_unreachable_nodes(const std::vector<NodePtr>& nodes);

        static std::vector<NodePtr> get_execution_order(const std::vector<NodePtr>& nodes);

    private:
        // std::unique_ptr<NodeFactory>     m_node_factory;
        // std::unique_ptr<ResourceFactory> m_resource_factory;
        std::shared_ptr<Context>         m_context;
    };
}