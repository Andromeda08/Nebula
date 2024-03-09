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

        std::map<int32_t, std::shared_ptr<EditorNode>> nodes;
        std::vector<Edge>                              edges;
    };
}