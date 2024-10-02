#include "VulkanRHI.hpp"

#include "VulkanExtension.hpp"
#include "VulkanLayer.hpp"
#include "VulkanUtil.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

VulkanRHI::VulkanRHI()
{
    vk::DynamicLoader dl;
    auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    createInstance();
    VULKAN_HPP_DEFAULT_DISPATCHER.init(mInstance);
}

void VulkanRHI::createInstance()
{
    auto appInfo = vk::ApplicationInfo().setApiVersion(mApiVersion);
    auto instanceInfo = vk::InstanceCreateInfo().setPApplicationInfo(&appInfo);

    // Instance Layers
    auto availableLayers = VulkanInstanceLayer::getDriverInstanceLayers();
    for (auto& layer : VulkanInstanceLayer::getRHIInstanceLayers())
    {
        if (int32_t index = VulkanLayer::findLayer(availableLayers, layer->layerName());
            index != -1)
        {
            layer->setSupported();
        }
        if (layer->isEnabled())
        {
            mInstanceLayers.push_back(layer->layerName());
        }
    }

    instanceInfo
        .setEnabledLayerCount(mInstanceLayers.size())
        .setPpEnabledLayerNames(mInstanceLayers.data());

    // Instance Extensions
    auto availableExtensions = VulkanInstanceExtension::getDriverInstanceExtensions();
    for (auto& extension : VulkanInstanceExtension::getRHIInstanceExtensions())
    {
        if (int32_t index = VulkanExtension::findExtension(availableExtensions, extension->extensionName());
            index != -1)
        {
            extension->setSupported();
        }
        if (extension->isEnabled())
        {
            mInstanceExtensions.push_back(extension->extensionName());
        }
    }

    instanceInfo
        .setEnabledExtensionCount(mInstanceExtensions.size())
        .setPpEnabledExtensionNames(mInstanceExtensions.data());

    if (const vk::Result result = vk::createInstance(&instanceInfo, nullptr, &mInstance);
        result != vk::Result::eSuccess)
    {
        VulkanRHIThrow(std::format("> Failed to create \x1b[31mvk::Instance\x1b[0m: {}", to_string(result)));
    }

    VulkanRHILogDebug(std::format("> Created \x1b[35mvk::Instance\x1b[0m [Layers: {} | Extensions: {}]", mInstanceLayers.size(), mInstanceExtensions.size()));
}
