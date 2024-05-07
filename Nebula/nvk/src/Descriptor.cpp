#include "Descriptor.hpp"
#include <nlog/nlog.hpp>

#ifdef NVK_VERBOSE_EXTRA
#include <iostream>
#endif

namespace Nebula::nvk
{
    vk::DescriptorType get_vk_descriptor_type(DescriptorType descriptor_type)
    {
        switch (descriptor_type)
        {
            case DescriptorType::eCombinedImageSampler:
                return vk::DescriptorType::eCombinedImageSampler;
            case DescriptorType::eStorageBuffer:
                return vk::DescriptorType::eStorageBuffer;
            case DescriptorType::eStorageImage:
                return vk::DescriptorType::eStorageImage;
            case DescriptorType::eSampledImage:
                return vk::DescriptorType::eSampledImage;
            case DescriptorType::eSampler:
                return vk::DescriptorType::eSampler;
            case DescriptorType::eUniformBuffer:
                return vk::DescriptorType::eUniformBuffer;
            case DescriptorType::eUniformBufferDynamic:
                return vk::DescriptorType::eUniformBufferDynamic;
            case DescriptorType::eAccelerationStructure:
                return vk::DescriptorType::eAccelerationStructureKHR;
            default:
                throw nlog::make_exception("Unknown descriptor type.");
        }
    }

    vk::DescriptorSetLayoutBinding make_descriptor_binding(DescriptorType type, uint32_t binding,
                                                           vk::ShaderStageFlags shader_stages, uint32_t count)
    {
        return vk::DescriptorSetLayoutBinding()
            .setBinding(binding)
            .setDescriptorCount(count)
            .setDescriptorType(get_vk_descriptor_type(type))
            .setStageFlags(shader_stages);
    }


    Descriptor::Descriptor(const DescriptorCreateInfo& create_info, const std::shared_ptr<Device>& device)
    : m_bindings(create_info.bindings), m_device(device), m_name(create_info.name), m_set_count(create_info.descriptor_set_count)
    {
        create_pool();
        create_layout();
        create_sets();
        if (create_info.ring_mode)
        {
            create_ring();
        }

        #ifdef NVK_VERBOSE_EXTRA
        std::cout << nlog::fmt_verbose("Created Descriptor: {} | # of Sets: {} | Ring Mode : {}",
                                       m_name, m_set_count, (m_set_ring ? "enabled" : "disabled")) << std::endl;
        #endif
    }

    Descriptor::~Descriptor()
    {
        m_device->handle().destroy(m_pool);
        m_device->handle().destroy(m_layout);

        #ifdef NVK_VERBOSE_EXTRA
        std::cout << nlog::fmt_verbose("Destroyed Descriptor: {} | # of Sets {}", m_name, m_set_count) << std::endl;
        #endif
    }

    void Descriptor::write(DescriptorWriteInfo& write_info)
    {
        for (auto& write : write_info.writes)
        {
            write.setDstSet(m_sets[write_info.set_index]);
        }

        if (!write_info.writes.empty())
        {
            m_device->handle().updateDescriptorSets(write_info.writes.size(), write_info.writes.data(), 0, nullptr);
        }
    }

    const vk::DescriptorSet& Descriptor::set(uint32_t index) const
    {
        if (index > m_sets.size())
        {
            throw nlog::make_exception<std::out_of_range>("Index {} out of range for descriptor sets", index);
        }

        return m_sets[index];
    }

    const vk::DescriptorSet& Descriptor::operator[](uint32_t index) const
    {
        return this->set(index);
    }

    void Descriptor::create_pool()
    {
        std::vector<vk::DescriptorPoolSize> pool_sizes;
        pool_sizes.reserve(m_bindings.size());
        for (const auto& binding : m_bindings)
        {
            pool_sizes.emplace_back(binding.descriptorType, binding.descriptorCount);
        }

        auto pool_create_info = vk::DescriptorPoolCreateInfo()
            .setMaxSets(m_set_count)
            .setPoolSizeCount(static_cast<uint32_t>(pool_sizes.size()))
            .setPPoolSizes(pool_sizes.data());

        if (const vk::Result result = m_device->handle().createDescriptorPool(&pool_create_info, nullptr, &m_pool);
            result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to create vk::DescriptorPool for nvk::Descriptor \"{}\" ({})",
                                       m_name, to_string(result));
        }

        m_device->name_object(
            std::format("{}: DescriptorPool", m_name),
            (uint64_t) m_pool.operator VkDescriptorPool(),
            vk::ObjectType::eDescriptorPool);
    }

    void Descriptor::create_layout()
    {
        auto create_info = vk::DescriptorSetLayoutCreateInfo()
            .setBindingCount(static_cast<uint32_t>(m_bindings.size()))
            .setPBindings(m_bindings.data());

        if (const vk::Result result = m_device->handle().createDescriptorSetLayout(&create_info, nullptr, &m_layout);
            result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to create vk::DescriptorSetLayout for nvk::Descriptor \"{}\" ({})",
                                       m_name, to_string(result));
        }

        m_device->name_object(
            std::format("{}: DescriptorLayout", m_name),
            (uint64_t) m_layout.operator VkDescriptorSetLayout(),
            vk::ObjectType::eDescriptorSetLayout);
    }

    void Descriptor::create_sets()
    {
        m_sets.resize(m_set_count);
        std::vector<vk::DescriptorSetLayout> layouts(m_set_count, m_layout);

        auto alloc_info = vk::DescriptorSetAllocateInfo()
            .setDescriptorPool(m_pool)
            .setDescriptorSetCount(m_set_count)
            .setPSetLayouts(layouts.data());

        if (const vk::Result result = m_device->handle().allocateDescriptorSets(&alloc_info, m_sets.data());
            result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to allocate {} vk::DescriptorSet(s) for nvk::Descriptor \"{}\" ({})",
                                       m_set_count, m_name, to_string(result));
        }

        for (size_t i = 0; i < m_sets.size(); i++)
        {
            m_device->name_object(
                std::format("{} Set #{}", m_name, i),
                (uint64_t) m_sets[i].operator VkDescriptorSet(),
                vk::ObjectType::eDescriptorSet);
        }
    }

    void Descriptor::create_ring()
    {
        m_set_ring = std::make_unique<DescriptorRing>(m_sets);
    }
}