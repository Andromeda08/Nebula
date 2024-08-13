#include "Buffer.hpp"
#include "Utilities.hpp"

namespace Nebula::nvk
{
    std::string to_string(const BufferType buffer_type)
    {
        switch (buffer_type)
        {
            using enum BufferType;
            case eAccelerationStructureStorage: return "AccelerationStructure";
            case eCustom:                       return "Custom";
            case eIndex:                        return "Index";
            case eShaderBindingTable:           return "ShaderBindingTable";
            case eStaging:                      return "Staging";
            case eStorage:                      return "Storage";
            case eUniform:                      return "Uniform";
            case eVertex:                       return "Vertex";
            default:                            return "Unknown";
        }
    }

    Buffer::Buffer(const BufferCreateInfo& create_info, const std::shared_ptr<Device>& device)
    : m_device(device), m_name(create_info.name), m_type(create_info.type)
    {
        auto base_flags = BufferTypeFlags::for_type(create_info.type);
        auto buffer_create_info = vk::BufferCreateInfo()
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSize(create_info.size)
            .setUsage(base_flags.usage_flags | create_info.extra_usage_flags);

        if (const vk::Result result = m_device->handle().createBuffer(&buffer_create_info, nullptr, &m_buffer);
            result != vk::Result::eSuccess)
        {
            throw make_exception("Failed to create Buffer \"{}\" ({})", m_name, to_string(result));
        }

        auto allocation_info = AllocationInfo()
            .set_buffer(m_buffer)
            .set_property_flags(base_flags.memory_flags | create_info.extra_memory_property_flags);

        m_allocation = m_device->allocate_memory(allocation_info);
        m_allocation->bind();

        auto address_info = vk::BufferDeviceAddressInfo().setBuffer(m_buffer);
        m_address = m_device->handle().getBufferAddress(&address_info);

        m_device->name_object(m_buffer, fmt::format("{} [{}]", m_name, to_string(m_type)), vk::ObjectType::eBuffer);

        if (m_type != BufferType::eStaging)
        {
            print_success("Created {} Buffer: {} [Size={}]", to_string(m_type), m_name, size());
        }
    }

    Buffer::~Buffer()
    {
        m_device->handle().destroy(m_buffer);
        m_allocation->free();

        if (m_type != BufferType::eStaging)
        {
            print_verbose("Destroyed {} Buffer: {}", to_string(m_type), m_name);
        }
    }

    void Buffer::copy_to_buffer(const Buffer& dst, const vk::CommandBuffer& command_buffer)
    {
         auto copy_region = vk::BufferCopy()
            .setSize(m_allocation->size)
            .setSrcOffset(m_allocation->offset)
            .setDstOffset(dst.m_allocation->offset);
        command_buffer.copyBuffer(m_buffer, dst.m_buffer, 1, &copy_region);
    }

    void Buffer::copy_to_image(const Image& dst, const vk::CommandBuffer& command_buffer)
    {
        auto img_props = dst.properties();
        auto copy_region = vk::BufferImageCopy()
            .setBufferOffset(m_allocation->offset)
            .setBufferRowLength(0)
            .setBufferImageHeight(0)
            .setImageSubresource(img_props.subresource_layers)
            .setImageOffset({ 0, 0, 0 })
            .setImageExtent({ img_props.extent.width, img_props.extent.height, 1 });
        command_buffer.copyBufferToImage(m_buffer, dst.image(), vk::ImageLayout::eTransferDstOptimal, 1, &copy_region);
    }

    Buffer::BufferTypeFlags Buffer::BufferTypeFlags::for_type(const BufferType buffer_type)
    {
        if (buffer_type == BufferType::eCustom)
        {
            return { {}, {} };
        }

        BufferTypeFlags result = {
            .memory_flags = {},
            .usage_flags = vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc,
        };

        switch (buffer_type)
        {
            using enum BufferType;
            using enum vk::BufferUsageFlagBits;
            using enum vk::MemoryPropertyFlagBits;

            case eAccelerationStructureStorage: {
                result.usage_flags  |= eAccelerationStructureStorageKHR;
                result.memory_flags |= eHostVisible | eHostCoherent;
                break;
            }
            case eIndex: {
                result.usage_flags  |= eIndexBuffer | eStorageBuffer | eAccelerationStructureBuildInputReadOnlyKHR;
                result.memory_flags |= eDeviceLocal;
                break;
            }
            case eShaderBindingTable: {
                result.usage_flags  |= eShaderBindingTableKHR | eTransferDst;
                result.memory_flags |= eDeviceLocal | eHostVisible;
                break;
            }
            case eStaging: {
                result.usage_flags  |= eTransferSrc;
                result.memory_flags |= eHostVisible | eHostCoherent;
                break;
            }
            case eStorage: {
                result.usage_flags  |= eStorageBuffer;
                result.memory_flags |= eHostVisible | eHostCoherent;
                break;
            }
            case eUniform: {
                result.usage_flags  |= eUniformBuffer;
                result.memory_flags |= eHostVisible | eHostCoherent;
                break;
            }
            case eVertex: {
                result.usage_flags  |= eVertexBuffer | eStorageBuffer | eAccelerationStructureBuildInputReadOnlyKHR;
                result.memory_flags |= eDeviceLocal;
                break;
            }
            default:
                break;
        }
        return result;
    }
}
