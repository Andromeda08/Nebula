#include "Device.hpp"
#include <format>
#include <stdexcept>
#include <vector>

#if defined(NVK_VERBOSE) || defined(NVK_VERBOSE_EXTRA)
#include <iostream>
#endif

namespace Nebula::nvk
{
    Allocation::Allocation(const std::variant<vk::Buffer, vk::Image>& user,
                           const vk::DeviceSize& device_size,
                           const vk::Device& device,
                           const uint32_t id)
    : size(device_size), m_user(user), m_device(device), m_id(id)
    {
    }

    void Allocation::bind()
    {
        if (std::holds_alternative<vk::Buffer>(m_user))
        {
            auto& buffer = std::get<vk::Buffer>(m_user);
            m_device.bindBufferMemory(buffer, memory, offset);
            return;
        }
        if (std::holds_alternative<vk::Image>(m_user))
        {
            auto& image = std::get<vk::Image>(m_user);
            m_device.bindImageMemory(image, memory, offset);
            return;
        }
        throw std::runtime_error("Failed to bind memory");
    }

    void* Allocation::map()
    {
        void* mapped_memory = nullptr;
        if (const vk::Result result = m_device.mapMemory(memory, offset, size, {}, &mapped_memory);
            result != vk::Result::eSuccess)
        {
            throw std::runtime_error(std::format("Failed to map memory for Allocation ID: {}", m_id));
        }
        return mapped_memory;
    }

    void Allocation::unmap()
    {
        m_device.unmapMemory(memory);
    }

    void Allocation::free()
    {
        if (!m_invalid)
        {
            m_device.free(memory);
            m_invalid = true;
        }
    }

    Device::Device(const vk::Instance& instance, const DeviceExtensions& device_extensions)
    : m_device_extensions(device_extensions)
    {
        select_device(instance);
        create_device();
    }

    std::shared_ptr<Allocation> Device::allocate_memory(const AllocationInfo& allocation_info)
    {
        vk::MemoryRequirements memory_requirements;
        if (std::holds_alternative<vk::Buffer>(allocation_info.target))
        {
            auto& buffer = std::get<vk::Buffer>(allocation_info.target);
            memory_requirements = m_device.getBufferMemoryRequirements(buffer);
        }
        if (std::holds_alternative<vk::Image>(allocation_info.target))
        {
            auto& image = std::get<vk::Image>(allocation_info.target);
            memory_requirements = m_device.getImageMemoryRequirements(image);
        }

        const uint32_t heap = find_memory_heap_index(memory_requirements.memoryTypeBits, allocation_info.property_flags);
        auto allocate_flags = vk::MemoryAllocateFlagsInfo()
            .setFlags(vk::MemoryAllocateFlagBits::eDeviceAddress);
        auto allocate_info = vk::MemoryAllocateInfo()
            .setAllocationSize(memory_requirements.size)
            .setMemoryTypeIndex(heap)
            .setPNext(&allocate_flags);

        auto id = static_cast<uint32_t>(m_allocations.size());
        auto allocation = std::make_shared<Allocation>(allocation_info.target, memory_requirements.size, m_device, id);

        if (const vk::Result result = m_device.allocateMemory(&allocate_info, nullptr, &allocation->memory);
            result != vk::Result::eSuccess)
        {
            throw std::runtime_error(std::format("Failed to allocate memory"));
        }

        #ifdef NVK_VERBOSE_EXTRA
        std::cout << std::format("[V++] Allocated memory of size {}", allocation->size) << std::endl;
        #endif

        m_allocations.push_back(allocation);
        return m_allocations.back();
    }

    void Device::name_object(const std::string& name, uint64_t handle, vk::ObjectType type) const
    {
        auto name_info = vk::DebugUtilsObjectNameInfoEXT()
            .setPObjectName(name.c_str())
            .setObjectHandle(handle)
            .setObjectType(type);

        if (const vk::Result result = m_device.setDebugUtilsObjectNameEXT(&name_info);
            result != vk::Result::eSuccess)
        {
            #ifdef NVK_VERBOSE
            throw std::runtime_error(std::format("Failed to name Vulkan Object {} of type {} as \"{}\"",
                                                 handle, to_string(type), name));
            #endif
        }
    }

    void Device::select_device(const vk::Instance& instance)
    {
        std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

        const auto candidate = std::ranges::find_if(devices,[&](const vk::PhysicalDevice& device){
            const vk::PhysicalDeviceProperties properties = device.getProperties();

            const std::vector<vk::ExtensionProperties> supported_extensions = device.enumerateDeviceExtensionProperties();
            const std::vector<const char*>& required_extensions = m_device_extensions.get_required_device_extensions();
            bool supports_requested_extensions = true;
            for (const auto& extension : required_extensions)
            {
                const auto it = std::ranges::find_if(supported_extensions, [&](const vk::ExtensionProperties& ext){
                    return std::string(ext.extensionName.data()) == std::string(extension);
                });
                if (it == std::end(supported_extensions))
                {
                    supports_requested_extensions = false;
                }
            }

            return supports_requested_extensions
                && Queue::has_queue(device, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)
                && Queue::has_queue(device, vk::QueueFlagBits::eCompute, vk::QueueFlagBits::eGraphics)
                && properties.deviceType != vk::PhysicalDeviceType::eIntegratedGpu;
        });

        if (candidate == std::end(devices))
        {
            throw std::runtime_error(std::format("Failed to find a suitable PhysicalDevice"));
        }

        m_physical_device = *candidate;
        m_physical_device_properties = m_physical_device.getProperties();

        #ifdef NVK_VERBOSE
        std::cout << std::format("[V] Selected PhysicalDevice : {}", m_physical_device_properties.deviceName.data()) << std::endl;
        #endif
    }

    void Device::create_device()
    {
        const std::vector<const char*>& extensions = m_device_extensions.get_required_device_extensions();

        auto q_cg = Queue::find_queue(m_physical_device, vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eGraphics, {}, true).value();
        auto q_async_c = Queue::find_queue(m_physical_device, vk::QueueFlagBits::eCompute, vk::QueueFlagBits::eGraphics, true).value();

        const float queue_priority = 1.0f;
        std::set<uint32_t> unique_queue_families { q_cg.family_index, q_async_c.family_index };

        std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
        for (const uint32_t fidx : unique_queue_families)
        {
            auto qci = vk::DeviceQueueCreateInfo()
                .setQueueFamilyIndex(fidx)
                .setQueueCount(1)
                .setPQueuePriorities(&queue_priority);
            queue_create_infos.push_back(qci);
        }

        auto create_info = vk::DeviceCreateInfo()
            .setEnabledExtensionCount(extensions.size())
            .setPpEnabledExtensionNames(extensions.data())
            .setQueueCreateInfoCount(queue_create_infos.size())
            .setPQueueCreateInfos(queue_create_infos.data())
            .setPEnabledFeatures(&m_device_extensions.features)
            .setPNext(&m_device_extensions.vulkan_11);

        if (const vk::Result result = m_physical_device.createDevice(&create_info, nullptr, &m_device);
            result != vk::Result::eSuccess)
        {
            throw std::runtime_error(std::format("Failed to create vk::Device ({})", to_string(result)));
        }

        #ifdef NVK_VERBOSE
        std::cout << "[V] Created vk::Device" << std::endl;
        #endif

        auto q0 = QueueCreateInfo()
            .set_queue_family_index(q_cg.family_index)
            .set_queue_index(0)
            .set_name("Queue [CG]");
        m_queue_general = std::make_unique<Queue>(q0, m_device);
        name_object(q0.name, (uint64_t) m_queue_general->queue.operator VkQueue(), vk::ObjectType::eQueue);

        auto q1 = QueueCreateInfo()
            .set_queue_family_index(q_async_c.family_index)
            .set_queue_index(0)
            .set_name("Queue [C]");
        m_queue_async_compute = std::make_unique<Queue>(q1, m_device);
        name_object(q1.name, (uint64_t) m_queue_async_compute->queue.operator VkQueue(), vk::ObjectType::eQueue);
    }

    uint32_t Device::find_memory_heap_index(uint32_t filter, vk::MemoryPropertyFlags property_flags) const
    {
        auto memory_properties = m_physical_device.getMemoryProperties();
        for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
        {
            if ((filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory heap");
    }

    MemoryUsage Device::get_memory_usage() const
    {
        vk::PhysicalDeviceMemoryProperties2         mem_props;
        vk::PhysicalDeviceMemoryBudgetPropertiesEXT mem_budget;
        mem_props.pNext = &mem_budget;
        m_physical_device.getMemoryProperties2(&mem_props);

        uint64_t memory_usage {0};
        uint64_t memory_budget {0};
        for (int32_t i = 0; i < mem_props.memoryProperties.memoryHeapCount; i++)
        {
            memory_usage  += mem_budget.heapUsage[i];
            memory_budget += mem_budget.heapBudget[i];
        }

        auto [ mu, mu_m ] = _convert_memory_size(memory_usage);
        auto [ mb, mb_m ] = _convert_memory_size(memory_budget);

        return {
            .budget = mb,
            .budget_coeff = mb_m,
            .usage = mu,
            .usage_coeff = mu_m,
        };
    }

    std::tuple<float, std::string> Device::_convert_memory_size(const uint64_t input_memory) const
    {
        std::tuple<float, std::string> result;

        static constexpr float kilobyte_coefficient = 1024.0f;
        static constexpr float megabyte_coefficient = kilobyte_coefficient * 1024.0f;
        static constexpr float gigabyte_coefficient = megabyte_coefficient * 1024.0f;

        auto memory = static_cast<float>(input_memory);
        if (memory < kilobyte_coefficient)
        {
            result = { memory, "B" };
        }
        else if (memory < megabyte_coefficient)
        {
            result = { memory / kilobyte_coefficient, "KB" };
        }
        else if (memory < gigabyte_coefficient)
        {
            result = { memory / megabyte_coefficient, "MB" };
        }
        else
        {
            result = { memory / gigabyte_coefficient, "GB" };
        }

        return result;
    }
}