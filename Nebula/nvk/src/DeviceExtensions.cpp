#include "DeviceExtensions.hpp"

#include <format>
#include <iostream>

namespace Nebula::nvk
{
    DeviceExtensions::DeviceExtensions()
    {
        features
            .setFillModeNonSolid(true)
            .setGeometryShader(true)
            .setSamplerAnisotropy(true)
            .setSampleRateShading(true)
            .setShaderInt64(true)
            .setTessellationShader(true);

        m_chain.emplace_back(vulkan_11.sType, &vulkan_11);

        vulkan_12
            .setBufferDeviceAddress(true)
            .setDescriptorIndexing(true)
            .setScalarBlockLayout(true)
            .setShaderFloat16(true)
            .setShaderInt8(true)
            .setTimelineSemaphore(true);
        m_chain.emplace_back(vulkan_12.sType, &vulkan_12);

        vulkan_13
            .setDynamicRendering(true)
            .setMaintenance4(true)
            .setSynchronization2(true);
        m_chain.emplace_back(vulkan_13.sType, &vulkan_13);

        m_device_extensions.insert(m_device_extensions.end(), {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
            VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
            VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
        });
    }

    DeviceExtensions& DeviceExtensions::enable_raytracing_features(bool value)
    {
        if (!value)
        {
            return *this;
        }
        as_features.setAccelerationStructure(true);
        m_chain.emplace_back(as_features.sType, &as_features);

        rt_pipeline.setRayTracingPipeline(true);
        m_chain.emplace_back(rt_pipeline.sType, &rt_pipeline);

        ray_query.setRayQuery(true);
        m_chain.emplace_back(ray_query.sType, &ray_query);

        m_device_extensions.insert(m_device_extensions.end(), {
            VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
            VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
            VK_KHR_RAY_QUERY_EXTENSION_NAME,
            VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        });

        return *this;
    }

    DeviceExtensions& DeviceExtensions::enable_mesh_shader_features(bool value)
    {
        if (!value)
        {
            return *this;
        }
        mesh_shader
            .setMeshShader(true)
            .setTaskShader(true);
        m_chain.emplace_back(mesh_shader.sType, &mesh_shader);
        m_device_extensions.push_back(VK_EXT_MESH_SHADER_EXTENSION_NAME);
        return *this;
    }

    DeviceExtensions& DeviceExtensions::set_chain()
    {
        void* p_next = nullptr;
        vk::StructureType s_type = vk::StructureType::eApplicationInfo;
        for (auto [sType, sPtr] : m_chain)
        {
            if (p_next == nullptr)
            {
                p_next = sPtr;
                s_type = sType;
                continue;
            }

            switch (sType)
            {
                using enum vk::StructureType;
                case ePhysicalDeviceVulkan11Features:
                    reinterpret_cast<vk::PhysicalDeviceVulkan11Features*>(sPtr)->setPNext(p_next);
                    break;
                case ePhysicalDeviceVulkan12Features:
                    reinterpret_cast<vk::PhysicalDeviceVulkan12Features*>(sPtr)->setPNext(p_next);
                    break;
                case ePhysicalDeviceVulkan13Features:
                    reinterpret_cast<vk::PhysicalDeviceVulkan13Features*>(sPtr)->setPNext(p_next);
                    break;
                case ePhysicalDeviceAccelerationStructureFeaturesKHR:
                    reinterpret_cast<vk::PhysicalDeviceAccelerationStructureFeaturesKHR*>(sPtr)->setPNext(p_next);
                    break;
                case ePhysicalDeviceRayTracingPipelineFeaturesKHR:
                    reinterpret_cast<vk::PhysicalDeviceRayTracingPipelineFeaturesKHR*>(sPtr)->setPNext(p_next);
                    break;
                case ePhysicalDeviceRayQueryFeaturesKHR:
                    reinterpret_cast<vk::PhysicalDeviceRayQueryFeaturesKHR*>(sPtr)->setPNext(p_next);
                    break;
                case ePhysicalDeviceMeshShaderFeaturesEXT:
                    reinterpret_cast<vk::PhysicalDeviceMeshShaderFeaturesEXT*>(sPtr)->setPNext(p_next);
                    break;
                default:
                    throw std::runtime_error(std::format("Unhandled structure type: {}", to_string(sType)));
            }

            std::cout << std::format("Chained: {} -> {} | {} -> {}", p_next, sPtr, to_string(s_type), to_string(sType)) << std::endl;

            s_type = sType;
            p_next = sPtr;
        }
        return *this;
    }

    const std::vector<const char*>& DeviceExtensions::get_required_device_extensions() const
    {
        return m_device_extensions;
    }

    void* DeviceExtensions::get_p_next() const
    {
        return m_chain.back().second;
    }
}