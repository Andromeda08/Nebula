#include "rt/TLAS.hpp"
#include <nlog/nlog.hpp>

#ifdef NVK_VERBOSE_EXTRA
#include <iostream>
#endif

namespace Nebula::nvk
{

    TLAS::TLAS(const TLASCreateInfo& create_info, const std::shared_ptr<Device>& device,
               const std::shared_ptr<CommandPool>& command_pool)
    : m_device(device), m_command_pool(command_pool), m_name(create_info.name)
    {
        build(create_info.instance_info);

        m_device->name_object(m_name,
                              (uint64_t) m_tlas.operator VkAccelerationStructureKHR(),
                              vk::ObjectType::eAccelerationStructureKHR);

        #ifdef NVK_VERBOSE_EXTRA
        std::cout << nlog::fmt_info("Created Top-Level AS: {}", m_create_info.name) << std::endl;
        #endif
    }

    void TLAS::build(const std::vector<TLASInstanceInfo>& instance_info)
    {
        m_instance_count = instance_info.size();

        // Instance Data
        #pragma region
        std::vector<vk::AccelerationStructureInstanceKHR> instances(instance_info.size());
        for (int32_t i = 0; i < instances.size(); i++)
        {
            auto& instance = instance_info[i];
            instances[i]
                .setAccelerationStructureReference(instance.blas_address)
                .setFlags(vk::GeometryInstanceFlagBitsKHR::eTriangleFacingCullDisable)
                .setInstanceShaderBindingTableRecordOffset(instance.hit_group)
                .setMask(instance.mask)
                .setTransform(instance.transform);
        }

        vk::DeviceSize instances_size = instances.size() * sizeof(vk::AccelerationStructureInstanceKHR);
        auto staging_create_info = BufferCreateInfo()
            .set_buffer_type(BufferType::eStaging)
            .set_name(std::format("{} TLAS Instance Data", m_name))
            .set_size(instances_size);
        auto staging_buffer = Buffer::create(staging_create_info, m_device);

        using enum vk::BufferUsageFlagBits;
        using enum vk::MemoryPropertyFlagBits;
        auto instance_create_info = BufferCreateInfo()
            .add_memory_property_flags(eDeviceLocal | eHostCoherent)
            .add_usage_flags(eTransferDst | eShaderDeviceAddress | eAccelerationStructureBuildInputReadOnlyKHR)
            .set_buffer_type(BufferType::eCustom)
            .set_size(instances_size)
            .set_name(std::format("{} TLAS Instance Data", m_name));
        m_instance_data = Buffer::create(instance_create_info, m_device);

        staging_buffer->set_data(instances.data());
//        m_command_pool->exec_single_time_command([&](const vk::CommandBuffer& command_buffer){
//            staging_buffer->copy_to_buffer(*m_instance_data, command_buffer);
//        });
        #pragma endregion

        // Acceleration Structure
        #pragma region
        auto instances_data = vk::AccelerationStructureGeometryInstancesDataKHR()
            .setArrayOfPointers(false)
            .setData(m_instance_data->address());

        auto geometry = vk::AccelerationStructureGeometryKHR()
            .setGeometryType(vk::GeometryTypeKHR::eInstances)
            .setGeometry(instancces_data);

        auto build_geometry_info = vk::AccelerationStructureBuildGeometryInfoKHR()
            .setFlags(vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace)
            .setGeometryCount(1)
            .setMode(vk::BuildAccelerationStructureModeKHR::eBuild)
            .setPGeometies(&geometry)
            .setType(vk::AccelerationStructureTypeKHR::eTopLevel);

        vk::AccelerationStructureBuildSizesInfoKHR build_sizes_info;
        m_device->handle().getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice,
                                                                 &build_geometry_info,
                                                                 &m_instance_count,
                                                                 &build_sizes_info);

        auto tlas_buffer_create_info = BufferCreateInfo()
            .set_buffer_type(BufferType::eAccelerationStructureStorage)
            .set_name(std::format("{} TLAS", m_name))
            .set_size(build_sizes_info.accelerationStructureSize);
        m_buffer = Buffer::create(tlas_buffer_create_info, m_device);

        auto create_info = vk::AccelerationStructureCreateInfoKHR()
            .setBuffer(m_buffer->buffer())
            .setOffset(0)
            .setSize(build_sizes_info.accelerationStructureSize)
            .setType(vk::AccelerationStructureTypeKHR::eTopLevel);

        if (vk::Result result = device->handle().createAcceerationStructureKHR(&create_info, nullptr, &m_tlas);
            result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to create Top-Level AS (name: {})", create_info.name);
        }

        auto as_staging_create_info = BufferCreateInfo()
            .set_buffer_type(BufferType::eStaging)
            .set_name(std::format("{} TLAS", m_name))
            .set_size(build_sizes_info.buildScratchSize);
        auto as_staging_buffer = Buffer::create(as_staging_create_info, m_device);

        build_geometry_info
            .setDstAccelerationStructure(m_tlas)
            .setScratchData(as_staging_buffer->address());

        auto build_range_info = vk::AccelerationStructureBuildRangeInfoKHR()
            .setPrimitiveCount(m_instance_count);
        std::vector<const vk::AccelerationStructureBuildRangeInfoKHR*> build_ranges = { &build_range_info };

//        m_command_pool->exec_single_time_command([&](const vk::CommandBuffer& command_buffer){
//            command_buffer.buildAccelerationStructuresKHR(1, &build_geometry_info, build_ranges.data());
//        });
        #pragma endregion

        m_command_pool->exec_single_time_command([&](const vk::CommandBuffer& command_buffer) {
            staging_buffer->copy_to_buffer(*m_instance_data, command_buffer);
            command_buffer.buildAccelerationStructuresKHR(1, &build_geometry_info, build_ranges.data());
        });
    }

    void TLAS::update(const TLASUpdateInfo& update_info)
    {
        m_command_pool->exec_single_time_command([&](const vk::CommandBuffer& command_buffer) {
           update(update_info, command_buffer);
        });
    }

    void TLAS::update(const TLASUpdateInfo& update_info, const vk::CommandBuffer& command_buffer)
    {
        throw nlog::make_exception("TLAS::update() is not implemented");
    }
}