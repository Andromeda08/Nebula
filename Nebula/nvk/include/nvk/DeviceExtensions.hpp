#pragma once

#include <vulkan/vulkan.hpp>

namespace Nebula::nvk
{
    struct DeviceExtensions
    {
        vk::PhysicalDeviceFeatures                          features {};
        vk::PhysicalDeviceVulkan11Features                  vulkan_11 {};
        vk::PhysicalDeviceVulkan12Features                  vulkan_12 {};
        vk::PhysicalDeviceVulkan13Features                  vulkan_13 {};

        // Ray Tracing Pipeline
        vk::PhysicalDeviceAccelerationStructureFeaturesKHR  as_features {};
        vk::PhysicalDeviceRayTracingPipelineFeaturesKHR     rt_pipeline {};
        vk::PhysicalDeviceRayQueryFeaturesKHR               ray_query {};

        // Mesh Shading Pipeline
        vk::PhysicalDeviceMeshShaderFeaturesEXT             mesh_shader {};

        // NV Specific Extensions
        vk::PhysicalDeviceOpticalFlowFeaturesNV             nv_optical_flow {};

        DeviceExtensions()
        {
            features
                .setFillModeNonSolid(true)
                .setGeometryShader(true)
                .setSamplerAnisotropy(true)
                .setSampleRateShading(true)
                .setShaderInt64(true)
                .setTessellationShader(true);

            vulkan_11.setPNext(&vulkan_12);

            vulkan_12
                .setBufferDeviceAddress(true)
                .setDescriptorIndexing(true)
                .setScalarBlockLayout(true)
                .setShaderFloat16(true)
                .setShaderInt8(true)
                .setTimelineSemaphore(true)
                .setPNext(&vulkan_13);

            vulkan_13
                .setDynamicRendering(true)
                .setMaintenance4(true)
                .setSynchronization2(true)
                .setPNext(&as_features);

            as_features
                .setAccelerationStructure(true)
                .setPNext(&rt_pipeline);
            rt_pipeline
                .setRayTracingPipeline(true)
                .setPNext(&ray_query);
            ray_query
                .setRayQuery(true)
                .setPNext(&mesh_shader);

            mesh_shader
                .setMeshShader(true)
                .setTaskShader(true);
                //.setPNext(&nv_optical_flow);

            nv_optical_flow
                .setOpticalFlow(true);

            m_device_extensions.insert(m_device_extensions.end(), {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
                VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
                VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
                VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
                VK_KHR_RAY_QUERY_EXTENSION_NAME,
                VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
                VK_EXT_MESH_SHADER_EXTENSION_NAME,
                //VK_NV_OPTICAL_FLOW_EXTENSION_NAME,
                VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
                VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
            });
        }

        inline DeviceExtensions& set_ray_tracing_features(bool value)
        {
//            if (!value) return *this;
//
//            if (m_p_next_for_device_create_info == nullptr)
//            {
//                m_p_next_for_device_create_info = &as_features;
//                vulkan_13.setPNext(&as_features);
//            }
//
//            as_features
//                .setAccelerationStructure(true)
//                .setPNext(&rt_pipeline);
//            rt_pipeline
//                .setRayTracingPipeline(true)
//                .setPNext(&ray_query);
//            ray_query
//                .setRayQuery(true);
//
//            m_device_extensions.insert(m_device_extensions.end(), {
//               VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
//               VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
//               VK_KHR_RAY_QUERY_EXTENSION_NAME,
//               VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
//            });
//
//            if (next_p_next != nullptr)
//            {
//                ray_query.setPNext(next_p_next);
//            }
//
//            next_p_next = &as_features;

            return *this;
        }

        inline DeviceExtensions& set_mesh_shading_features(bool value)
        {
//            if (!value) return *this;
//
//            if (m_p_next_for_device_create_info == nullptr)
//            {
//                m_p_next_for_device_create_info = &mesh_shader;
//                vulkan_13.setPNext(&mesh_shader);
//            }
//
//            mesh_shader
//                .setMeshShader(true)
//                .setTaskShader(true);
//
//            m_device_extensions.push_back(VK_EXT_MESH_SHADER_EXTENSION_NAME);
//
//            if (next_p_next != nullptr)
//            {
//                mesh_shader.setPNext(next_p_next);
//            }
//
//            next_p_next = &mesh_shader;

            return *this;
        }

        inline DeviceExtensions& set_optical_flow_features(bool value)
        {
//            if (!value) return *this;
//
//            if (m_p_next_for_device_create_info == nullptr)
//            {
//                m_p_next_for_device_create_info = &nv_optical_flow;
//                vulkan_13.setPNext(&nv_optical_flow);
//            }
//
//            nv_optical_flow
//                .setOpticalFlow(true);
//
//            m_device_extensions.push_back(VK_NV_OPTICAL_FLOW_EXTENSION_NAME);
//
//            if (next_p_next != nullptr)
//            {
//                nv_optical_flow.setPNext(next_p_next);
//            }
//
//            next_p_next = &nv_optical_flow;

            return *this;
        }

        const std::vector<const char*>& get_required_device_extensions() const
        {
            return m_device_extensions;
        }

        void* get_device_p_next() const
        {
            return m_p_next_for_device_create_info;
        }

    private:
        void*                       next_p_next {nullptr};
        void*                       m_p_next_for_device_create_info {nullptr};
        std::vector<const char*>    m_device_extensions;
    };
}