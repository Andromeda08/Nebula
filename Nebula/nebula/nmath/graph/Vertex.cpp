#include "Vertex.hpp"

namespace Nebula::nmath::graph
{
    Vertex::Vertex(const std::string& name)
    {
        m_name = (name.empty())
            ? std::format("Vertex {}", to_string(m_uuid))
            : name;
    }

    bool Vertex::make_directed_edge(const std::shared_ptr<Vertex>& a, const std::shared_ptr<Vertex>& b)
    {
        if (a->uuid() == b->uuid()) return false;

        a->m_outgoing_edges.push_back(b);
        b->m_incoming_edges.push_back(a);

        return true;
    }

    bool Vertex::delete_directed_edge(const std::shared_ptr<Vertex>& a, const std::shared_ptr<Vertex>& b)
    {
        if (a->uuid() == b->uuid()) return false;

        const auto b_find = std::ranges::find_if(a->m_outgoing_edges, [&](auto& v){ return v->uuid() == b->uuid(); });
        if (b_find == std::end(a->m_outgoing_edges)) return false;

        const auto a_find = std::ranges::find_if(b->m_incoming_edges, [&](auto& v){ return v->uuid() == a->uuid(); });
        if (a_find == std::end(b->m_incoming_edges)) return false;

        a->m_outgoing_edges.erase(b_find);
        b->m_incoming_edges.erase(a_find);

        return true;
    }
}