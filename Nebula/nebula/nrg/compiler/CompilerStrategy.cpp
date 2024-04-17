#include "CompilerStrategy.hpp"
#include <nmath/algorithm/BFS.hpp>
#include <nmath/algorithm/TopologicalSort.hpp>

namespace Nebula::nrg
{
    CompilerStrategy::CompilerStrategy(const std::shared_ptr<Context>& context)
    : m_context(context)
    {
        m_node_factory = std::make_unique<NodeFactory>(m_context);
        m_resource_factory = std::make_unique<ResourceFactory>(m_context);
    }

    std::vector<std::shared_ptr<EditorNode>>
    CompilerStrategy::filter_unreachable_nodes(const std::vector<std::shared_ptr<EditorNode>>& nodes)
    {
        std::vector<std::shared_ptr<EditorNode>> result;

        std::shared_ptr<EditorNode> root_node;
        for (const auto& node : nodes)
        {
            if (node->type() == NodeType::eSceneDataProvider)
            {
                root_node = node;
            }
        }

        if (!root_node)
        {
            throw std::runtime_error("Graph must contain a SceneDataProvider node");
        }
        
        auto reachable_node_ids = nmath::algorithm::BFS::execute(root_node);
        for (const auto& node : nodes)
        {
            if (reachable_node_ids.contains(node->uuid()))
            {
                result.push_back(node);
            }
        }

        return result;
    }

    std::vector<std::shared_ptr<EditorNode>>
    CompilerStrategy::get_execution_order(const std::vector<std::shared_ptr<EditorNode>>& nodes)
    {
        std::vector<std::shared_ptr<EditorNode>> result;

        std::vector<std::shared_ptr<nmath::graph::Vertex>> input(nodes.begin(), nodes.end());
        auto tsort = std::make_unique<nmath::algorithm::TopologicalSort>(input);
        try {
            auto tsort_result = tsort->execute();

            for (int32_t i = 0; result.size() != nodes.size(); i++)
            {
                auto node = std::ranges::find_if(nodes, [&](const auto& it){ return it->uuid() == tsort_result[i]->uuid(); });
                if (node == std::end(nodes))
                {
                    throw std::runtime_error("Somehow a node has been lost");
                }
                result.push_back(*node);
            }
        } catch (const std::runtime_error& e) {
            throw e;
        }

        // !!! Important !!!
        // An execution order must always end with a "Present" node
        if (const auto last_node = result.back();
            last_node->type() != NodeType::ePresent)
        {
            const auto present_node = std::ranges::find_if(result, [](const auto& n){
                return n->type() == NodeType::ePresent;
            });
            std::rotate(present_node, present_node + 1, std::end(result));
        }

        return result;
    }
}