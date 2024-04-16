#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <nmath/Transform.hpp>
#include <nscene/geometry/Mesh.hpp>
#include <nvk/rt/TLAS.hpp>

namespace Nebula::ns
{
    struct ObjectDescription
    {
        uint64_t index_buffer;
        uint64_t vertex_buffer;
    };

    struct ObjectPushConstant
    {
        glm::mat4 model       = glm::mat4(1.0f);
        glm::vec4 solid_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    };

    struct Object
    {
        std::shared_ptr<Mesh>   mesh;
        std::string             name {"Unknown Object"};
        uint32_t                rt_hit_group {0};
        glm::vec4               solid_color {0.5f, 0.5f, 0.5f, 1.0f};
        nmath::Transform        transform {};

        inline ObjectDescription get_description() const
        {
            auto bptrs = mesh->get_buffer_pointers();
            return {
                .index_buffer = bptrs.index_buffer,
                .vertex_buffer = bptrs.vertex_buffer,
            };
        }

        inline ObjectPushConstant get_push_constants() const
        {
            return {
                .model = transform.model(),
                .solid_color = solid_color,
            };
        }

        inline nvk::TLASInstanceInfo get_tlas_instance_info() const
        {
            return {
                .blas_address = mesh->bottom_level_as()->address(),
                .hit_group    = rt_hit_group,
                .mask         = 0xff,
                .transform    = transform.model3x4(),
            };
        }
    };
}