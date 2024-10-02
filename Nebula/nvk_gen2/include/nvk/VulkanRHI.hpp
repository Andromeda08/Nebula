#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

class VulkanDevice;

class VulkanRHI
{
public:
    VulkanRHI();

    ~VulkanRHI() = default;

    void createInstance();
    void selectDevice();

private:
    const uint32_t                  mApiVersion { VK_API_VERSION_1_3 };

    vk::Instance                    mInstance;
    std::vector<const char*>        mInstanceExtensions;
    std::vector<const char*>        mInstanceLayers;

    std::shared_ptr<VulkanDevice>   mDevice;
};