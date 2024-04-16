#pragma once

#include <memory>
#include <string>
#include <nscene/geometry/Geometry.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Command.hpp>
#include <nvk/Device.hpp>
#include <nvk/rt/BLAS.hpp>

namespace Nebula::ns
{
    struct MeshCreateInfo
    {
        Geometry*   p_geometry {nullptr};
        std::string name {"Unknown Mesh"};
        bool        create_meshlet {false};
        bool        create_blas {false};
        uint32_t    meshlet_vertex_limit {64};
        uint32_t    meshlet_index_limit {126};

        MeshCreateInfo() = default;

        MeshCreateInfo& set_p_geometry(Geometry* geometry)
        {
            p_geometry = geometry;
            return *this;
        }

        MeshCreateInfo& set_name(const std::string& value)
        {
            name = value;
            return *this;
        }

        MeshCreateInfo& set_meshlet_vertex_limit(uint32_t value)
        {
            meshlet_vertex_limit = value;
            return *this;
        }

        MeshCreateInfo& set_meshlet_index_limit(uint32_t value)
        {
            meshlet_index_limit = value;
            return *this;
        }
    };

    struct MeshBufferPointers
    {
        uint64_t index_buffer;
        uint64_t vertex_buffer;
    };

    class Mesh
    {
    public:
        Mesh(const std::string& name);

        Mesh(const MeshCreateInfo& create_info,
             const std::shared_ptr<nvk::Device>& device,
             const std::shared_ptr<nvk::CommandPool>& command_pool);

        virtual ~Mesh() = default;

        virtual void update(const vk::CommandBuffer& command_buffer);

        virtual void draw(const vk::CommandBuffer& command_buffer) const;

        const std::string& name() const { return m_name; }
        const nvk::Buffer& vertex_buffer() const { return *m_vertex_buffer; }
        const nvk::Buffer& index_buffer() const { return *m_index_buffer; }
        const std::shared_ptr<nvk::BLAS> bottom_level_as() const { return m_blas; }

        inline MeshBufferPointers get_buffer_pointers() const
        {
            return {
                .index_buffer = m_index_buffer->address(),
                .vertex_buffer = m_vertex_buffer->address(),
            };
        }

    protected:
        const std::string               m_name;
        uint32_t                        m_vertex_count {0};
        std::shared_ptr<nvk::Buffer>    m_vertex_buffer;
        uint32_t                        m_index_count {0};
        std::shared_ptr<nvk::Buffer>    m_index_buffer;
        std::shared_ptr<nvk::BLAS>      m_blas;
    };
}