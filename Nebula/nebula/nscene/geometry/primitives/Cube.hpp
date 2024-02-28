#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include <nscene/Vertex.hpp>
#include <nscene/geometry/Geometry.hpp>

namespace Nebula::ns
{
    class Cube : public Geometry
    {
    public:
        explicit Cube(float scale = 0.5f);

        ~Cube() override = default;

    private:
        static std::vector<Vertex> generate_vertices(float scale);

        static std::vector<uint32_t> generate_indices()
        {
            return {
                0, 1, 2, 2, 3, 0,
                4, 5, 6, 6, 7, 4,
                8, 9, 10, 10, 11, 8,
                12, 13, 14, 14, 15, 12,
                16, 17, 18, 18, 19, 16,
                20, 21, 22, 22, 23, 20
            };
        }
    };
}
