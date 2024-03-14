#pragma once

#include <memory>
#include <vector>
#include <uuid.h>
#include <nmath/graph/Vertex.hpp>

namespace Nebula::nmath::algorithm
{
    using namespace graph;

    class TopologicalSort
    {
    public:
        explicit TopologicalSort(const std::vector<std::shared_ptr<Vertex>>& vertices);

        std::vector<std::shared_ptr<Vertex>> execute();

    private:
        const std::vector<std::shared_ptr<Vertex>>& m_vertices;
    };
}