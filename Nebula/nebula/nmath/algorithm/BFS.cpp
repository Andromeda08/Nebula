#include "BFS.hpp"
#include <queue>

namespace Nebula::nmath::algorithm
{
    std::set<uuids::uuid> BFS::execute(const std::shared_ptr<Vertex>& root)
    {
        std::set<uuids::uuid> visited;
        std::queue<std::shared_ptr<Vertex>> Q;

        Q.push(root);
        visited.insert(root->uuid());

        while (!Q.empty())
        {
            const auto current = Q.front();
            Q.pop();

            for (const auto& w : current->get_outgoing_edges())
            {
                if (!visited.contains(w->uuid()))
                {
                    visited.insert(w->uuid());
                    Q.push(w);
                }
            }
        }

        return visited;
    }
}