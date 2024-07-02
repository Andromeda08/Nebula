#include "rt/ShaderBindingTable.hpp"
#include "Utilities.hpp"

namespace Nebula::nvk
{
    ShaderBindingTable::ShaderBindingTable(const ShaderBindingTableCreateInfo& create_info,
                                           const std::shared_ptr<Device>& device)
    : m_device(device), m_pipeline(create_info.pipeline), m_miss_count(create_info.miss_count)
    , m_hit_count(create_info.hit_count), m_callable_count(create_info.callable_count)
    {
        vk::PhysicalDeviceRayTracingPipelinePropertiesKHR rt_props;
        get_ray_tracing_properties(&rt_props);

        uint32_t shgr_handle_size = rt_props.shaderGroupHandleSize;
        uint32_t shgr_base_alignment = rt_props.shaderGroupBaseAlignment;
        uint32_t handle_count = 1 + m_miss_count + m_hit_count + m_callable_count;

        uint32_t aligned_handle_size = align_up(shgr_handle_size, rt_props.shaderGroupHandleAlignment);

        uint32_t rgen_stride = align_up(aligned_handle_size, shgr_base_alignment);
        m_rgen = vk::StridedDeviceAddressRegionKHR()
            .setSize(rgen_stride)
            .setStride(rgen_stride);

        m_miss = vk::StridedDeviceAddressRegionKHR()
            .setSize(align_up(m_miss_count * aligned_handle_size, shgr_base_alignment))
            .setStride(aligned_handle_size);

        m_hit = vk::StridedDeviceAddressRegionKHR()
            .setSize(align_up(m_hit_count * aligned_handle_size, shgr_base_alignment))
            .setStride(aligned_handle_size);

        m_callable = vk::StridedDeviceAddressRegionKHR()
            .setSize(align_up(m_callable_count * aligned_handle_size, shgr_base_alignment))
            .setStride(aligned_handle_size);

        uint32_t data_size = handle_count * shgr_handle_size;
        std::vector<uint8_t> handles(data_size);
        if (auto result = m_device->handle().getRayTracingShaderGroupHandlesKHR(m_pipeline, 0, handle_count, data_size, handles.data());
            result != vk::Result::eSuccess)
        {
            throw make_exception("Failed to fetch ray tracing shader group handles");
        }

        vk::DeviceSize sbt_size = m_rgen.size + m_miss.size + m_hit.size + m_callable.size;
        auto sbt_buffer_create_info = BufferCreateInfo()
            .set_buffer_type(BufferType::eShaderBindingTable)
            .set_name(create_info.name)
            .set_size(sbt_size);
        m_buffer = Buffer::create(sbt_buffer_create_info, m_device);

        auto sbt_address = m_buffer->address();
        m_rgen.setDeviceAddress(sbt_address);
        m_miss.setDeviceAddress(sbt_address + m_rgen.size);
        m_hit.setDeviceAddress(sbt_address + m_rgen.size + m_miss.size);
        m_callable.setDeviceAddress(sbt_address + m_rgen.size + m_miss.size + m_hit.size);

        auto get_handle = [&](uint32_t i) { return handles.data() + i * shgr_handle_size; };

        void* mapped_memory = m_buffer->m_allocation->map();
        uint8_t* p_sbt = reinterpret_cast<uint8_t*>(mapped_memory);
        uint8_t* p_data = nullptr;
        uint32_t handle_idx = 0;

        p_data = p_sbt;
        std::memcpy(p_data, get_handle(handle_idx++), shgr_handle_size);

        p_data = reinterpret_cast<uint8_t*>(mapped_memory) + m_rgen.size;
        for (uint32_t i = 0; i < m_miss_count; i++)
        {
            std::memcpy(p_data, get_handle(handle_idx++), shgr_handle_size);
            p_data += m_miss.stride;
        }

        p_data = reinterpret_cast<uint8_t*>(mapped_memory) + m_rgen.size + m_miss.size;
        for (uint32_t i = 0; i < m_hit_count; i++)
        {
            std::memcpy(p_data, get_handle(handle_idx++), shgr_handle_size);
            p_data += m_hit.stride;
        }


        p_data = reinterpret_cast<uint8_t*>(mapped_memory) + m_rgen.size + m_miss.size + m_hit.size;
        for (uint32_t i = 0; i < m_callable_count; i++)
        {
            std::memcpy(p_data, get_handle(handle_idx++), shgr_handle_size);
            p_data += m_callable.stride;
        }


        m_buffer->m_allocation->unmap();
    }

    void ShaderBindingTable::get_ray_tracing_properties(vk::PhysicalDeviceRayTracingPipelinePropertiesKHR* p_rt_props)
    {
        vk::PhysicalDeviceProperties2 props2;
        props2.pNext = p_rt_props;
        m_device->physical_device().getProperties2(&props2);
    }
}