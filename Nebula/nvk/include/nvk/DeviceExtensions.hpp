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

        DeviceExtensions();

        DeviceExtensions& enable_raytracing_features(bool value = true);

        DeviceExtensions& enable_mesh_shader_features(bool value = true);

        DeviceExtensions& set_chain();

        void* get_p_next() const;

        const std::vector<const char*>& get_required_device_extensions() const;

    private:
        std::vector<std::pair<vk::StructureType, void*>> m_chain;
        std::vector<const char*> m_device_extensions;
    };
}