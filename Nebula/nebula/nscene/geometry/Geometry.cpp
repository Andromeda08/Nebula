#include "Geometry.hpp"

namespace Nebula::ns
{
    Geometry::Geometry(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : m_vertices(vertices), m_vertex_count(static_cast<uint32_t>(vertices.size()))
    , m_indices(indices), m_index_count(static_cast<uint32_t>(indices.size()))
    {
    }

    const std::vector<Vertex>& Geometry::vertices() const
    {
        return m_vertices;
    }

    const std::vector<uint32_t>& Geometry::indices() const
    {
        return m_indices;
    }

    uint32_t Geometry::vertex_count() const
    {
        return m_vertex_count;
    }

    uint32_t Geometry::index_count() const
    {
        return m_index_count;
    }
}