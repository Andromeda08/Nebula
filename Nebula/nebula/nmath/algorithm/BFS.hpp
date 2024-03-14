#pragma once

#include <memory>
#include <set>
#include <uuid.h>
#include <nmath/graph/Vertex.hpp>

namespace Nebula::nmath::algorithm
{
    using namespace graph;

    struct BFS
    {
        /**
         * @return Set of vertex IDs which were visited during execution.
         */
        static std::set<uuids::uuid> execute(const std::shared_ptr<Vertex>& root);
    };
}