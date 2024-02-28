#include "Mesh.hpp"

namespace Nebula::ns
{
    Mesh::Mesh(const MeshCreateInfo& create_info,
               const std::shared_ptr<nvk::Device>& device,
               const std::shared_ptr<nvk::CommandPool>& command_pool)
    : m_name(create_info.name), m_index_count(create_info.p_geometry->index_count())
    {
        auto name = std::format("[Mesh] {}", m_name);

        auto vb_create_info = nvk::BufferCreateInfo()
            .set_buffer_type(nvk::BufferType::eVertex)
            .set_name(name)
            .set_size(sizeof(Vertex) * create_info.p_geometry->vertex_count());
        m_vertex_buffer = std::make_shared<nvk::Buffer>(vb_create_info, device);

        auto vbstgci = nvk::BufferCreateInfo()
            .set_buffer_type(nvk::BufferType::eStaging)
            .set_name(name)
            .set_size(vb_create_info.size);
        auto vb_staging = std::make_shared<nvk::Buffer>(vbstgci, device);
        vb_staging->set_data(create_info.p_geometry->vertices().data());

        auto ib_create_info = nvk::BufferCreateInfo()
            .set_buffer_type(nvk::BufferType::eIndex)
            .set_name(name)
            .set_size(sizeof(uint32_t) * create_info.p_geometry->index_count());
        m_index_buffer = std::make_shared<nvk::Buffer>(ib_create_info, device);

        auto ibstgci = nvk::BufferCreateInfo()
            .set_buffer_type(nvk::BufferType::eStaging)
            .set_name(name)
            .set_size(ib_create_info.size);
        auto ib_staging = std::make_shared<nvk::Buffer>(ibstgci, device);
        ib_staging->set_data(create_info.p_geometry->indices().data());

        command_pool->exec_single_time_command([&](const vk::CommandBuffer& command_buffer){
            vb_staging->copy_to_buffer(*m_vertex_buffer, command_buffer);
            ib_staging->copy_to_buffer(*m_index_buffer, command_buffer);
        });
    }

    void Mesh::update(const vk::CommandBuffer& command_buffer)
    {

    }

    void Mesh::draw(const vk::CommandBuffer& command_buffer) const
    {
        static const std::vector<vk::DeviceSize> offsets = { 0 };
        command_buffer.bindVertexBuffers(0, 1, &m_vertex_buffer->buffer(), offsets.data());
        command_buffer.bindIndexBuffer(m_index_buffer->buffer(), 0, vk::IndexType::eUint32);
        command_buffer.drawIndexed(m_index_count, 1, 0, 0, 0);
    }
}
