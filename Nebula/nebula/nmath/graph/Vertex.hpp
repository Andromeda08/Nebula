#pragma once

#include <vector>
#include <string>
#include <uuid.h>

namespace Nebula::nmath::graph
{
    class Vertex
    {
    public:
        static bool make_directed_edge(const std::shared_ptr<Vertex>& from, const std::shared_ptr<Vertex>& to);

        static bool delete_directed_edge(const std::shared_ptr<Vertex>& from, const std::shared_ptr<Vertex>& to);

        explicit Vertex(const std::string& name = "");

        virtual ~Vertex() = default;

        const uuids::uuid& uuid() const { return m_uuid; }

        const std::string& name() const { return m_name; }

        const std::vector<std::shared_ptr<Vertex>>& get_incoming_edges() const
        {
            return m_incoming_edges;
        }

        int32_t in_degree() const
        {
            return static_cast<int32_t>(m_incoming_edges.size());
        }

        const std::vector<std::shared_ptr<Vertex>>& get_outgoing_edges() const
        {
            return m_outgoing_edges;
        }

        int32_t out_degree() const
        {
            return static_cast<int32_t>(m_outgoing_edges.size());
        }

    private:
        uuids::uuid m_uuid = uuids::uuid_system_generator{}();
        std::string m_name;

        std::vector<std::shared_ptr<Vertex>> m_incoming_edges;
        std::vector<std::shared_ptr<Vertex>> m_outgoing_edges;
    };
}