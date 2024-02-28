#include "Sphere.hpp"

namespace Nebula::ns
{
    Sphere::Sphere(float radius, int32_t tesselation): Geometry()
    {
        m_vertices = generate_vertices(tesselation, tesselation, radius);
        m_vertex_count = m_vertices.size();
        m_indices = generate_indices(tesselation, tesselation);
        m_index_count = m_indices.size();
    }

    std::vector<Vertex> Sphere::generate_vertices(int32_t stack_count, int32_t sector_count, float radius)
    {
        std::vector<glm::vec3> vertices, normals;
        std::vector<glm::vec2> uvs;

        float sector_step = 2.0f * std::numbers::pi_v<float> / (float) sector_count;
        float stack_step  = std::numbers::pi_v<float> / (float) stack_count;
        float sector_angle, stack_angle;

        float r_inv = 1.0f / radius;

        for (int32_t i = 0; i <= stack_count; i++)
        {
            float x, y, z, xy;
            stack_angle = std::numbers::pi_v<float> / 2 - (float) i * stack_step;
            xy = radius * cosf(stack_angle);
            z = radius * sinf(stack_angle);

            for (int32_t j = 0; j <= sector_count; j++)
            {
                sector_angle = (float) j * sector_step;

                x = xy * cosf(sector_angle);
                y = xy * sinf(sector_angle);

                vertices.emplace_back(x, y, z);
                normals.emplace_back(x * r_inv, y * r_inv, z * r_inv);
                uvs.emplace_back((float) j / (float) sector_count, (float) i / (float) stack_count);
            }
        }

        std::vector<Vertex> result;
        for (uint32_t i = 0; i < vertices.size(); i++)
        {
            result.push_back({ vertices[i], normals[i], uvs[i] });
        }
        return result;
    }

    std::vector<uint32_t> Sphere::generate_indices(int32_t stack_count, int32_t sector_count)
    {
        std::vector<uint32_t> indices;
        uint32_t k1, k2;

        for (int32_t i = 0; i < stack_count; i++)
        {
            k1 = i * (sector_count + 1);
            k2 = k1 + sector_count + 1;

            for (int32_t j = 0; j < sector_count; j++, k1++, k2++)
            {
                if (i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                if (i != (stack_count - 1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
        return indices;
    }
}