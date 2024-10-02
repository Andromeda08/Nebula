#include "VulkanLayer.hpp"

int32_t VulkanLayer::findLayer(const std::vector<vk::LayerProperties>& layers, const char* layerName)
{
    for (int32_t i = 0; i < layers.size(); i++)
    {
        if (!std::strcmp(layers[i].layerName.operator const char*(), layerName))
        {
            return i;
        }
    }
    return -1;
}

VulkanInstanceLayerVector VulkanInstanceLayer::getRHIInstanceLayers()
{
    VulkanInstanceLayerVector instanceLayers;

    #define ADD_LAYER(LAYER_NAME, REQUESTED) \
        instanceLayers.push_back(std::make_unique<VulkanInstanceLayer>(LAYER_NAME, REQUESTED));

    #ifdef VULKAN_RHI_DEBUG
    ADD_LAYER("VK_LAYER_KHRONOS_validation", true);
    #else
    ADD_LAYER("VK_LAYER_KHRONOS_validation", false);
    #endif

    #undef ADd_LAYER

    return instanceLayers;
}

std::vector<vk::LayerProperties> VulkanInstanceLayer::getDriverInstanceLayers()
{
    return vk::enumerateInstanceLayerProperties();
}
