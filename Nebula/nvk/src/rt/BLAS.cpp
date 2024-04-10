#include "rt/BLAS.hpp"
#include <nlog/nlog.hpp>
#include <vector>

namespace Nebula::nvk
{
    BLAS::BLAS(const BLASCreateInfo& create_info,
               const std::shared_ptr<Device>& device,
               const std::shared_ptr<CommandPool>& command_pool)
    : m_device(device)
    , m_create_info(create_info)
    {
        auto tri_data = vk::AccelerationStructureGeometryTrianglesDataKHR()
            .setIndexData(create_info.index_buffer->address())
            .setIndexType(vk::IndexType::eUint32)
            .setVertexData(create_info.vertex_buffer->address())
            .setVertexFormat(vk::Format::eR32G32B32A32Sfloat)
            .setVertexStride(create_info.vertex_stride)
            .setMaxVertex(create_info.vertex_count);

        auto geometry = vk::AccelerationStructureGeometryKHR()
            .setGeometryType(vk::GeometryTypeKHR::eTriangles)
            .setGeometry(tri_data);

        vk::AccelerationStructureBuildSizesInfoKHR    build_sizes_info {};
        vk::AccelerationStructureBuildGeometryInfoKHR build_geometry_info {};
        build_geometry_info
            .setFlags(vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace)
            .setGeometryCount(1)
            .setPGeometries(&geometry)
            .setMode(vk::BuildAccelerationStructureModeKHR::eBuild)
            .setType(vk::AccelerationStructureTypeKHR::eBottomLevel);

        const uint32_t triangle_count = create_info.index_count / 3;
        device->handle().getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice,
                                                               &build_geometry_info,
                                                               &triangle_count,
                                                               &build_sizes_info);

        auto buffer_create_info = BufferCreateInfo()
            .set_buffer_type(BufferType::eAccelerationStructureStorage)
            .set_size(build_sizes_info.accelerationStructureSize)
            .set_name(create_info.name);
        m_buffer = std::make_shared<Buffer>(buffer_create_info, device);

        auto as_create_info = vk::AccelerationStructureCreateInfoKHR()
            .setBuffer(m_buffer->buffer())
            .setOffset(0)
            .setSize(build_sizes_info.accelerationStructureSize)
            .setType(vk::AccelerationStructureTypeKHR::eBottomLevel);
        if (vk::Result result = device->handle().createAccelerationStructureKHR(&as_create_info, nullptr, &m_blas);
            result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to create Bottom Level AS (name: {})", create_info.name);
        }

        auto address_info = vk::AccelerationStructureDeviceAddressInfoKHR()
            .setAccelerationStructure(m_blas);
        m_address = device->handle().getAccelerationStructureAddressKHR(&address_info);

        auto scratch_buffer_create_info = BufferCreateInfo()
            .set_buffer_type(BufferType::eStaging)
            .set_size(build_sizes_info.accelerationStructureSize)
            .set_name(create_info.name);
        auto scratch_buffer = std::make_shared<Buffer>(scratch_buffer_create_info, device);

        build_geometry_info
            .setDstAccelerationStructure(m_blas)
            .setScratchData(scratch_buffer->address());

        auto build_range_info = vk::AccelerationStructureBuildRangeInfoKHR()
            .setPrimitiveCount(triangle_count);
        std::vector<const vk::AccelerationStructureBuildRangeInfoKHR*> build_ranges = { &build_range_info };

        command_pool->exec_single_time_command([&](const vk::CommandBuffer& command_buffer){
           command_buffer.buildAccelerationStructuresKHR(1, &build_geometry_info, build_ranges.data());
        });
    }

}