#include "HairModel.hpp"
#include <fmt/format.h>
#include <stdexcept>

namespace Nebula::nhair
{
    HairModel::HairModel(const std::string& file_path,
                         const std::shared_ptr<nvk::Device>& device,
                         const std::shared_ptr<nvk::CommandPool>& command_pool)
    : m_file_path(file_path), m_device(device), m_command_pool(command_pool)
    {
        load_file();
        process_vertices();
        process_strands();
        create_buffers();
        create_initial_position_buffers();

        m_gx = static_cast<uint32_t>(std::floor(strand_count() / 32));
    }

    void HairModel::load_file()
    {
        m_hair_file.LoadFromFile(m_file_path.c_str());
    }

    void HairModel::process_vertices()
    {
        float* hair_points = m_hair_file.GetPointsArray();
        for (int32_t i = 0; i < m_hair_file.GetHeader().point_count * 3; i += 3) {
            m_vertices.emplace_back(glm::vec4(hair_points[i], hair_points[i + 1], hair_points[i + 2], 1.0f));
        }
    }

    void HairModel::process_strands()
    {
        if (m_hair_file.GetSegmentsArray() != nullptr)
        {
            uint16_t* segments_array = m_hair_file.GetSegmentsArray();
            for (int32_t i = 0; i < m_hair_file.GetHeader().hair_count; i++) {
                m_strand_vertex_counts.push_back(segments_array[i]);
            }
        }

        std::span vertex_span { m_vertices };

        int32_t vtx_offset = 0;
        for (int32_t i = 0; i < m_hair_file.GetHeader().hair_count; i++)
        {
            auto strand_vertex_count = static_cast<int32_t>((m_strand_vertex_counts.empty())
                ? m_hair_file.GetHeader().d_segments + 1
                : m_strand_vertex_counts[i] + 1);

            // 1. Strand
            Strand strand {
                .id = i,
                .point_count = strand_vertex_count,
                .vertices = vertex_span.subspan(vtx_offset, strand_vertex_count),
            };
            m_strands.push_back(strand);

            // 2. Process Strandlets
            const int32_t strandlet_size = Strandlet::s_MAX_STRANDLET_SIZE;
            int32_t strandlet_count = std::ceil(static_cast<double>(strand_vertex_count) / static_cast<double>(strandlet_size));
            std::vector<Strandlet> strandlets;
            for (int32_t j = 0; j < strandlet_count; j++)
            {
                int32_t point_count = (j != strandlet_count - 1) ? 32 : (strand_vertex_count - (j * strandlet_size));
                Strandlet strandlet {
                    .strand_id = i,
                    .point_count = point_count,
                    .vertices = strand.vertices.subspan((j * strandlet_size), point_count),
                };
                strandlets.push_back(strandlet);
                m_strandlets.push_back(strandlet);
            }

            // 3. Strand Description
            StrandDescription strand_description {
                .strand_id = i,
                .point_count = strand.point_count,
                .strandlet_count = strandlet_count,
                .vertex_offset = vtx_offset,
            };
            m_strand_descriptions.push_back(strand_description);

            vtx_offset += strand_vertex_count;
        }
    }

    void HairModel::create_buffers()
    {
        auto name = fmt::format("[Hair] {}", m_file_path);
        auto vb_create_info = nvk::BufferCreateInfo()
            .set_buffer_type(nvk::BufferType::eStorage)
            .set_name(name)
            .set_size(sizeof(Vertex) * m_vertices.size());
        m_vertex_buffer = nvk::Buffer::create_with_data(vb_create_info, m_vertices.data(), m_device, m_command_pool);

        auto sdb_create_info = nvk::BufferCreateInfo()
            .set_buffer_type(nvk::BufferType::eStorage)
            .set_name(name)
            .set_size(sizeof(StrandDescription) * m_strand_descriptions.size());
        m_strand_descriptions_buffer = nvk::Buffer::create_with_data(sdb_create_info, m_strand_descriptions.data(), m_device, m_command_pool);
    }

    void HairModel::draw(const vk::CommandBuffer& command_buffer) const
    {
        command_buffer.drawMeshTasksEXT(m_gx, 1, 1);
    }

    void HairModel::update()
    {
        m_current_position_buffer = (m_current_position_buffer + 1) % 2;
    }

    void HairModel::create_initial_position_buffers()
    {
        auto create_info = nvk::BufferCreateInfo()
            .set_buffer_type(nvk::BufferType::eStorage)
            .set_size(m_vertex_buffer->size());
        int32_t i = -1;
        for (auto& buffer : m_position_buffers)
        {
            auto name = fmt::format("[Hair | {}] Position Buffer #{}", m_file_path, i++);
            create_info.set_name(name);
            buffer = nvk::Buffer::create_with_data(create_info, m_vertices.data(), m_device, m_command_pool);
        }
    }
}