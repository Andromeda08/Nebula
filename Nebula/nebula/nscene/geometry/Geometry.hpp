#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include <nscene/Vertex.hpp>

namespace Nebula::ns
{
    class Geometry
    {
    public:
        Geometry() = default;

        Geometry(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

        virtual ~Geometry() = default;

        const std::vector<Vertex>&   vertices() const;

        const std::vector<uint32_t>& indices()  const;

        uint32_t vertex_count() const;

        uint32_t index_count()  const;

    protected:
        std::vector<Vertex>   m_vertices {};
        uint32_t              m_vertex_count {0};
        std::vector<uint32_t> m_indices {};
        uint32_t              m_index_count {0};
    };
}