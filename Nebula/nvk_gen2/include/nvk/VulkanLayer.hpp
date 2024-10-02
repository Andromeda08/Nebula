#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

using VulkanInstanceLayerVector = std::vector<std::unique_ptr<class VulkanInstanceLayer>>;

class VulkanLayer
{
public:
    explicit VulkanLayer(const char* layerName, bool requested = false)
    : mLayerName(layerName)
    , mRequested(requested)
    , mSupported(false)
    , mActive(false)
    {}

    virtual ~VulkanLayer() = default;

    inline void setRequested()
    {
        mRequested = true;
        mActive = canActivate();
    }
    inline void setSupported()
    {
        mSupported = true;
        mActive = canActivate();
    }

    inline const char* layerName() const { return mLayerName; }
    inline bool isSupported() const { return mSupported; }

    inline bool isEnabled() const
    {
        return mSupported && mRequested && mActive;
    }

    inline bool canActivate() const
    {
        return mSupported && mRequested;
    }

    static int32_t findLayer(const std::vector<vk::LayerProperties>& layers, const char* layerName);

protected:
    const char* mLayerName;
    bool        mRequested;
    bool        mSupported;
    bool        mActive;
};

class VulkanInstanceLayer : public VulkanLayer
{
public:
    explicit VulkanInstanceLayer(const char* layerName, bool requested = false)
    : VulkanLayer(layerName, requested)
    {}

    static VulkanInstanceLayerVector        getRHIInstanceLayers();
    static std::vector<vk::LayerProperties> getDriverInstanceLayers();
};
