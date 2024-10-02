#include "VulkanExtension.hpp"

int32_t VulkanExtension::findExtension(const std::vector<vk::ExtensionProperties>& extensions, const char* extensionName)
{
    for (int32_t i = 0; i < extensions.size(); i++)
    {
        if (!std::strcmp(extensions[i].extensionName.operator const char*(), extensionName))
        {
            return i;
        }
    }
    return -1;
}

/**
 * Vulkan Instance Extensions
 */

VulkanInstanceExtensionVector VulkanInstanceExtension::getRHIInstanceExtensions()
{
    VulkanInstanceExtensionVector result;
    result.push_back(std::make_unique<VulkanInstanceExtension>(VK_KHR_SURFACE_EXTENSION_NAME, true));
    return result;
}

std::vector<vk::ExtensionProperties> VulkanInstanceExtension::getDriverInstanceExtensions()
{
    return vk::enumerateInstanceExtensionProperties();
}

/**
 * Vulkan Device Extensions
 */

// Vulkan 1.1 Core Features
class Vulkan11Extensions : public VulkanDeviceExtension
{
public:
    explicit Vulkan11Extensions()
    : VulkanDeviceExtension(nullptr, "", true, VK_API_VERSION_1_1)
    {}

    void postSupportCheck() override
    {
        if (!canActivate()) return;

        mActive = true;
        mVulkan11 = vk::PhysicalDeviceVulkan11Features();
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override
    {
        addToPNext(deviceCreateInfo, mVulkan11);
    }

private:
    vk::PhysicalDeviceVulkan11Features mVulkan11;
};

// Vulkan 1.2 Core Features
class Vulkan12Extensions : public VulkanDeviceExtension
{
public:
    explicit Vulkan12Extensions()
    : VulkanDeviceExtension(nullptr, "", true, VK_API_VERSION_1_2)
    {}

    void postSupportCheck() override
    {
        if (!canActivate()) return;

        mActive = true;
        mVulkan12 = vk::PhysicalDeviceVulkan12Features()
            .setBufferDeviceAddress(true)
            .setDescriptorIndexing(true)
            .setScalarBlockLayout(true)
            .setShaderFloat16(true)
            .setShaderInt8(true)
            .setTimelineSemaphore(true)
            .setHostQueryReset(true);
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override
    {
        addToPNext(deviceCreateInfo, mVulkan12);
    }

private:
    vk::PhysicalDeviceVulkan12Features mVulkan12;
};

// Vulkan 1.1 Core Features
class Vulkan13Extensions : public VulkanDeviceExtension
{
public:
    explicit Vulkan13Extensions()
    : VulkanDeviceExtension(nullptr, "", true, VK_API_VERSION_1_3)
    {}

    void postSupportCheck() override
    {
        if (!canActivate()) return;

        mActive = true;
        mVulkan13 = vk::PhysicalDeviceVulkan13Features()
            .setDynamicRendering(true)
            .setMaintenance4(true)
            .setSynchronization2(true);
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override
    {
        addToPNext(deviceCreateInfo, mVulkan13);
    }

private:
    vk::PhysicalDeviceVulkan13Features mVulkan13;
};

// VK_KHR_acceleration_structure
class VulkanKHRAccelerationStructureExtension : public VulkanDeviceExtension
{
public:
    explicit VulkanKHRAccelerationStructureExtension(VulkanDevice* device, bool requested = false)
    : VulkanDeviceExtension(device, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, requested)
    {}

    void postSupportCheck() override
    {
        if (!canActivate()) return;

        mActive = true;
        mAccelerationStructureFeatures.setAccelerationStructure(true);
    }

    void preCreateDevice(vk::DeviceCreateInfo &deviceCreateInfo) override
    {
        addToPNext(deviceCreateInfo, mAccelerationStructureFeatures);
    }

private:
    vk::PhysicalDeviceAccelerationStructureFeaturesKHR mAccelerationStructureFeatures;
};

// VK_KHR_raytracing_pipeline
class VulkanKHRRayTracingPipelineExtension : public VulkanDeviceExtension
{
public:
    explicit VulkanKHRRayTracingPipelineExtension(VulkanDevice* device, bool requested = false)
        : VulkanDeviceExtension(device, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, requested)
    {}

    void postSupportCheck() override
    {
        if (!canActivate()) return;

        mActive = true;
        mRayTracingPipelineFeatures.setRayTracingPipeline(true);
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override
    {
        addToPNext(deviceCreateInfo, mRayTracingPipelineFeatures);
    }

private:
    vk::PhysicalDeviceRayTracingPipelineFeaturesKHR mRayTracingPipelineFeatures;
};

// VK_KHR_ray_query
class VulkanKHRRayQueryExtension : public VulkanDeviceExtension
{
public:
    explicit VulkanKHRRayQueryExtension(VulkanDevice* device, bool requested = false)
        : VulkanDeviceExtension(device, VK_KHR_RAY_QUERY_EXTENSION_NAME, requested)
    {}

    void postSupportCheck() override
    {
        if (!canActivate()) return;

        mActive = true;
        mRayQueryFeatures.setRayQuery(true);
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override
    {
        addToPNext(deviceCreateInfo, mRayQueryFeatures);
    }

private:
    vk::PhysicalDeviceRayQueryFeaturesKHR mRayQueryFeatures;
};

// VK_KHR_mesh_shader
class VulkanEXTMeshShaderExtension : public VulkanDeviceExtension
{
public:
    explicit VulkanEXTMeshShaderExtension(VulkanDevice* device, bool requested = false)
        : VulkanDeviceExtension(device, VK_EXT_MESH_SHADER_EXTENSION_NAME, requested)
    {}

    void postSupportCheck() override
    {
        if (!canActivate()) return;

        mActive = true;
        mMeshShaderFeatures
            .setMeshShader(true)
            .setTaskShader(true);
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override
    {
        addToPNext(deviceCreateInfo, mMeshShaderFeatures);
    }

private:
    vk::PhysicalDeviceMeshShaderFeaturesEXT mMeshShaderFeatures;
};

// VK_EXT_descriptor_buffer
class VulkanEXTDescriptorBufferExtension : public VulkanDeviceExtension
{
public:
    explicit VulkanEXTDescriptorBufferExtension(VulkanDevice* device, bool requested = false)
    : VulkanDeviceExtension(device, VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME, requested)
    {}

    void postSupportCheck() override
    {
        if (!canActivate()) return;

        mActive = true;
        mDescriptorBufferFeatures
            .setDescriptorBuffer(true)
            .setDescriptorBufferPushDescriptors(true);
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override
    {
        addToPNext(deviceCreateInfo, mDescriptorBufferFeatures);
    }

private:
    vk::PhysicalDeviceDescriptorBufferFeaturesEXT mDescriptorBufferFeatures;
};

VulkanDeviceExtensionVector VulkanDeviceExtension::getRHIDeviceExtensions(VulkanDevice* device)
{
    VulkanDeviceExtensionVector deviceExtensions;

    #define ADD_EXTENSION(EXTENSION_NAME, REQUESTED, PROMOTED_VERSION) \
        deviceExtensions.push_back(std::make_unique<VulkanDeviceExtension>(device, EXTENSION_NAME, REQUESTED, PROMOTED_VERSION));

    #define ADD_CUSTOM(TYPE, REQUESTED) \
        deviceExtensions.push_back(std::make_unique<TYPE>(device, REQUESTED));

    #define ADD_CORE(TYPE) \
        deviceExtensions.push_back(std::make_unique<TYPE>());

    // Vulkan Core features
    // TODO: Consider checking API version before adding these, although only 1.3 is supported
    ADD_CORE(Vulkan11Extensions);
    ADD_CORE(Vulkan12Extensions);
    ADD_CORE(Vulkan13Extensions);

    // Simple extensions
    ADD_EXTENSION(VK_KHR_SWAPCHAIN_EXTENSION_NAME,                  true, VulkanExtension::sExtensionNotPromoted);
    ADD_EXTENSION(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,              true, VulkanExtension::sExtensionNotPromoted);
    ADD_EXTENSION(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,            true, VulkanExtension::sExtensionNotPromoted);

    // Raytracing
    ADD_EXTENSION(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,   true, VulkanExtension::sExtensionNotPromoted);
    ADD_CUSTOM(VulkanKHRAccelerationStructureExtension,             true);
    ADD_CUSTOM(VulkanKHRRayTracingPipelineExtension,                true);
    ADD_CUSTOM(VulkanKHRRayQueryExtension,                          true);

    // Mesh shading pipeline
    ADD_CUSTOM(VulkanEXTMeshShaderExtension,                        true);

    // Custom extensions
    ADD_CUSTOM(VulkanEXTDescriptorBufferExtension,                  true);

    #undef ADD_EXTENSION
    #undef ADD_CUSTOM
    #undef ADD_CORE

    return deviceExtensions;
}

std::vector<vk::ExtensionProperties> VulkanDeviceExtension::getDriverDeviceExtensions(vk::PhysicalDevice physicalDevice)
{
    return physicalDevice.enumerateDeviceExtensionProperties();
}