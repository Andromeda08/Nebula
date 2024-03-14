#include "TopologicalSort.hpp"

#include <map>
#include <stdexcept>
#include <queue>

namespace Nebula::nmath::algorithm
{
    TopologicalSort::TopologicalSort(const std::vector<std::shared_ptr<Vertex>>& vertices)
    : m_vertices(vertices)
    {
    }

    std::vector<std::shared_ptr<Vertex>> TopologicalSort::execute()
    {
        std::map<uuids::uuid, int32_t> in_degrees;
        for (const auto& vertex : m_vertices)
        {
            in_degrees.emplace(vertex->uuid(), vertex->in_degree());
        }

        std::vector<std::shared_ptr<Vertex>> T;
        std::queue<std::shared_ptr<Vertex>>  Q;

        for (const auto& vertex : m_vertices)
        {
            if (in_degrees[vertex->uuid()] == 0)
            {
                Q.push(vertex);
            }
        }

        while (!Q.empty())
        {
            auto v = Q.front();
            Q.pop();
            T.push_back(v);

            for (const auto& w : v->get_outgoing_edges())
            {
                auto w_id = w->uuid();

                in_degrees[w_id]--;
                if (in_degrees[w_id] == 0)
                {
                    auto i = std::ranges::find_if(m_vertices, [w_id](const auto& it){ return it->uuid() == w_id; });
                    Q.push(*i);
                }
            }
        }

        for (const auto& vertex : m_vertices)
        {
            if (in_degrees[vertex->uuid()] != 0)
            {
                throw std::runtime_error("Given graph was not acyclic.");
            }
        }

        return T;
    }
}