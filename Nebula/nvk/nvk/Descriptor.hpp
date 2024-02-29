#pragma once

#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "Buffer.hpp"
#include "Device.hpp"
#include "Utility.hpp"

namespace Nebula::nvk
{
    enum class DescriptorType
    {
        eCombinedImageSampler,
        eStorageBuffer,
        eStorageImage,
        eSampledImage,
        eSampler,
        eUniformBuffer,
        eUniformBufferDynamic,
        eAccelerationStructure,
    };

    struct DescriptorCreateInfo;

    struct DescriptorWriteInfo;

    class DescriptorRing;

    class Descriptor
    {
    public:
        NVK_DISABLE_COPY(Descriptor);

        Descriptor(const DescriptorCreateInfo& create_info, const std::shared_ptr<Device>& device);

        ~Descriptor();

        void write(DescriptorWriteInfo& write_info);

        const vk::DescriptorSet& set(uint32_t index) const;

        const vk::DescriptorSet& operator[](uint32_t index) const;

        DescriptorRing& get_ring() const { return *m_set_ring; }

        const vk::DescriptorSetLayout& layout() const { return m_layout; }

        uint32_t set_count() const { return m_sets.size(); }

    private:
        void create_pool();

        void create_layout();

        void create_sets();

        void create_ring();

        std::unique_ptr<DescriptorRing>             m_set_ring; // Only if ring_mode was enabled

        std::vector<vk::DescriptorSet>              m_sets;
        std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
        vk::DescriptorSetLayout                     m_layout;
        vk::DescriptorPool                          m_pool;

        std::shared_ptr<Device>                     m_device;
        const std::string                           m_name {"Unknown"};
        const uint32_t                              m_set_count {0};
    };

    vk::DescriptorType get_vk_descriptor_type(DescriptorType descriptor_type);

    vk::DescriptorSetLayoutBinding make_descriptor_binding(DescriptorType type, uint32_t binding,
                                                           vk::ShaderStageFlags shader_stages, uint32_t count);

    class DescriptorRing : public Ring<vk::DescriptorSet>
    {
    public:
        NVK_DISABLE_COPY(DescriptorRing);

        explicit DescriptorRing(const std::vector<vk::DescriptorSet>& sets)
        : Ring(sets.size())
        {
            for_each([&](vk::DescriptorSet& set, size_t i){
                set = sets[i];
            });
        }

        ~DescriptorRing() override = default;
    };

    struct DescriptorCreateInfo {
        std::vector<vk::DescriptorSetLayoutBinding> bindings {};
        std::string                                 name {"Unknown"};
        uint32_t                                    descriptor_set_count {1};
        bool                                        ring_mode {false};

        DescriptorCreateInfo() = default;

        inline DescriptorCreateInfo& add(DescriptorType type, uint32_t binding, vk::ShaderStageFlags shader_stages, uint32_t count = 1)
        {
            bindings.push_back(make_descriptor_binding(type, binding, shader_stages, count));
            return *this;
        }

        inline DescriptorCreateInfo& set_count(const uint32_t value)
        {
            descriptor_set_count = value;
            return *this;
        }

        inline DescriptorCreateInfo& set_name(const std::string& value)
        {
            name = value;
            return *this;
        }

        inline DescriptorCreateInfo& enable_ring_mode(const bool value = true)
        {
            ring_mode = value;
            return *this;
        }
    };

    struct DescriptorWriteInfo
    {
        std::vector<vk::WriteDescriptorSet>                         writes;
        std::vector<vk::WriteDescriptorSetAccelerationStructureKHR> as_infos;
        std::vector<vk::DescriptorBufferInfo>                       buffer_infos;
        std::vector<vk::DescriptorImageInfo>                        image_infos;
        uint32_t                                                    set_index;

        inline DescriptorWriteInfo& set_set_index(const uint32_t index)
        {
            set_index = index;
            return *this;
        }

        inline DescriptorWriteInfo& add_acceleration_structure(uint32_t binding, uint32_t as_count,
                                                               const vk::AccelerationStructureKHR* p_as, uint32_t count = 1)
        {
            as_infos.emplace_back(as_count, p_as);

            auto write = vk::WriteDescriptorSet()
                .setDstBinding(binding)
                .setDescriptorCount(count)
                .setDescriptorType(vk::DescriptorType::eAccelerationStructureKHR)
                .setDstArrayElement(0)
                .setPNext(&as_infos.back());

            writes.push_back(write);

            return *this;
        }


        inline DescriptorWriteInfo& add_uniform_buffer(uint32_t binding, const vk::DescriptorBufferInfo& buffer_info, uint32_t count = 1)
        {
            buffer_infos.push_back(buffer_info);

            const auto* dbiptr = &buffer_infos.back();
            //assert(dbiptr->offset == buffer.offset());
            //assert(dbiptr->range == range);

            auto write = vk::WriteDescriptorSet()
                .setDstBinding(binding)
                .setDescriptorCount(count)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDstArrayElement(0)
                .setPBufferInfo(&buffer_info);

            writes.push_back(write);

            return *this;
        }

        inline DescriptorWriteInfo& add_combined_image_sampler(uint32_t binding, const vk::DescriptorImageInfo& image_info, uint32_t count = 1)
        {
            auto write = vk::WriteDescriptorSet()
                .setDstBinding(binding)
                .setDescriptorCount(count)
                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setDstArrayElement(0)
                .setPImageInfo(&image_info);

            writes.push_back(write);

            return *this;
        }

        inline DescriptorWriteInfo& add_storage_image(uint32_t binding, const vk::DescriptorImageInfo& image_info, uint32_t count = 1)
        {
            auto write = vk::WriteDescriptorSet()
                .setDstBinding(binding)
                .setDescriptorCount(count)
                .setDescriptorType(vk::DescriptorType::eStorageImage)
                .setDstArrayElement(0)
                .setPImageInfo(&image_info);

            writes.push_back(write);

            return *this;
        }

        inline DescriptorWriteInfo& add_storage_buffer(uint32_t binding, const Buffer& buffer, vk::DeviceSize range, uint32_t count = 1)
        {
            auto info = buffer_infos.emplace_back(buffer.buffer(), buffer.offset(), range);

            auto write = vk::WriteDescriptorSet()
                .setDstBinding(binding)
                .setDescriptorCount(count)
                .setDescriptorType(vk::DescriptorType::eStorageBuffer)
                .setDstArrayElement(0)
                .setPBufferInfo(&buffer_infos.back());

            writes.push_back(write);

            return *this;
        }
    };
}