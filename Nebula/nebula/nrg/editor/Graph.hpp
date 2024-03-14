#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <vector>
#include <nrg/editor/Edge.hpp>
#include <nrg/editor/EditorNode.hpp>

namespace Nebula::nrg
{
    struct Graph
    {
        void reset()
        {
            edges.clear();
            nodes.clear();
        }

        std::vector<std::shared_ptr<EditorNode>> get_nodes_vector()
        {
            std::vector<std::shared_ptr<EditorNode>> nodes_vector;
            for (const auto& [k, v] : nodes)
            {
                nodes_vector.push_back(v);
            }
            return nodes_vector;
        }

        std::map<int32_t, std::shared_ptr<EditorNode>> nodes;
        std::vector<Edge>                              edges;
    };
}