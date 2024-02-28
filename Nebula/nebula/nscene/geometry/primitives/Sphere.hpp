#pragma once

#include <cstdint>
#include <numbers>
#include <vector>
#include <glm/glm.hpp>
#include <nscene/Vertex.hpp>
#include <nscene/geometry/Geometry.hpp>

namespace Nebula::ns
{
    class Sphere : public Geometry
    {
    public:
        explicit Sphere(float radius = 1.0f, int32_t tesselation = 60);

        ~Sphere() override = default;

    private:
        static std::vector<Vertex> generate_vertices(int32_t stack_count, int32_t sector_count, float radius = 1.0f);

        static std::vector<uint32_t> generate_indices(int32_t stack_count, int32_t sector_count);
    };
}