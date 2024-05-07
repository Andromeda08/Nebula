#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <span>
#include <vector>
#include <cyCore.h>
#include <cyHairFile.h>
#include <glm/glm.hpp>
#include <nscene/geometry/Mesh.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Context.hpp>

namespace Nebula::nhair
{
    struct Vertex
    {
        glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    };

    // CPU Only
    struct Strand
    {
        int32_t             id;
        int32_t             point_count;
        std::span<Vertex>   vertices;

    };

    // CPU Only
    struct Strandlet
    {
        int32_t             strand_id;
        int32_t             point_count;
        std::span<Vertex>   vertices;

        static constexpr int32_t s_MAX_STRANDLET_SIZE = 32;
    };

    struct StrandDescription
    {
        int32_t strand_id {0};
        int32_t point_count {0};
        int32_t strandlet_count {0};
        int32_t vertex_offset {0};
    };

    struct HairBufferAddresses
    {
        uint64_t vertex_buffer;
        uint64_t strand_descriptions_buffer;
    };

    class HairModel
    {
    public:
        explicit HairModel(const std::string& file_path,
                           const std::shared_ptr<nvk::Device>& device,
                           const std::shared_ptr<nvk::CommandPool>& command_pool);

        inline HairBufferAddresses get_hair_buffer_addresses() const
        {
            return {
                .vertex_buffer = m_position_buffers[m_current_position_buffer]->address(),
                .strand_descriptions_buffer = m_strand_descriptions_buffer->address(),
            };
        }

        void draw(const vk::CommandBuffer& command_buffer) const;

        void update();

        int32_t vertex_count() const { return static_cast<int32_t>(m_vertices.size()); }

        int32_t strand_count() const { return static_cast<int32_t>(m_strands.size()); }

        const nvk::Buffer& vertex_buffer() const { return *m_vertex_buffer; }

        const nvk::Buffer& strand_descriptions_buffer() const { return *m_strand_descriptions_buffer; }


        ~HairModel() = default;

    private:
        void load_file();

        void process_vertices();

        void process_strands();

        void create_buffers();

        void create_initial_position_buffers();

        std::string                         m_file_path;
        cyHairFile                          m_hair_file;

        std::vector<Vertex>                 m_vertices;
        std::vector<int32_t>                m_strand_vertex_counts;
        std::vector<Strand>                 m_strands;
        std::vector<Strandlet>              m_strandlets;
        std::vector<StrandDescription>      m_strand_descriptions;

        std::shared_ptr<nvk::Buffer>        m_strand_descriptions_buffer;
        std::shared_ptr<nvk::Buffer>        m_vertex_buffer;

        uint32_t                            m_gx;

        std::shared_ptr<nvk::Device>        m_device;
        std::shared_ptr<nvk::CommandPool>   m_command_pool;

        int32_t                                     m_current_position_buffer {0};
        std::array<std::shared_ptr<nvk::Buffer>, 2> m_position_buffers;

    public:
        const std::array<std::shared_ptr<nvk::Buffer>, 2>& position_buffers() const { return m_position_buffers; }

        const nvk::Buffer& position_buffer(size_t i) const { return *m_position_buffers[i % 2]; }
    };
}