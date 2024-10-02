#pragma once

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

class VulkanDevice;

using VulkanInstanceExtensionVector = std::vector<std::unique_ptr<class VulkanInstanceExtension>>;
using VulkanDeviceExtensionVector   = std::vector<std::unique_ptr<class VulkanDeviceExtension>>;

template <typename Chain_t, typename Struct_t>
static void addToPNext(Chain_t& existing, Struct_t& added)
{
    added.setPNext((void*) existing.pNext);
    existing.setPNext((void*) &added);
}

class VulkanExtension
{
public:
    // Value of mPromotedVersion in case the extension hasn't been promoted to Vulkan Core yet.
    static constexpr uint32_t sExtensionNotPromoted = std::numeric_limits<uint32_t>::max();

    VulkanExtension(const char* extensionName, bool requested, uint32_t promotedVersion)
    : mExtensionName(extensionName)
    , mPromotedVersion(promotedVersion)
    , mCore(false)
    , mSupported(false)
    , mRequested(requested)
    , mActive(false)
    {}

    virtual ~VulkanExtension() = default;

    inline void setCore(uint32_t apiVersion) { mCore = (apiVersion >= mPromotedVersion); }
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

    inline const char* extensionName() const { return mExtensionName; }
    inline bool isSupported() const { return mSupported; }
    inline bool isCore() const { return mCore; }

    inline bool isEnabled() const
    {
        return mSupported && mRequested && mActive;
    }

    inline bool canActivate() const
    {
        return mSupported && mRequested;
    }

    static int32_t findExtension(const std::vector<vk::ExtensionProperties>& extensions, const char* extensionName);

protected:
    const char* mExtensionName;

    uint32_t    mPromotedVersion;
    bool        mCore;

    bool        mSupported;
    bool        mRequested;
    bool        mActive;
};

class VulkanInstanceExtension : public VulkanExtension
{
public:
    VulkanInstanceExtension(const char* extensionName, bool requested, uint32_t promotedVersion = VulkanExtension::sExtensionNotPromoted)
    : VulkanExtension(extensionName, requested, promotedVersion)
    {}

    ~VulkanInstanceExtension() override = default;

    static VulkanInstanceExtensionVector        getRHIInstanceExtensions();
    static std::vector<vk::ExtensionProperties> getDriverInstanceExtensions();
};

class VulkanDeviceExtension : public VulkanExtension
{
public:
    VulkanDeviceExtension(VulkanDevice* device, const char* extensionName, bool requested, uint32_t promotedVersion = VulkanExtension::sExtensionNotPromoted)
    : VulkanExtension(extensionName, requested, promotedVersion)
    , mDevice(device)
    , mRequirementsPassed(false)
    {}

    ~VulkanDeviceExtension() override = default;

    virtual void postSupportCheck() {}

    virtual void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) {}

    static VulkanDeviceExtensionVector          getRHIDeviceExtensions(VulkanDevice* device);
    static std::vector<vk::ExtensionProperties> getDriverDeviceExtensions(vk::PhysicalDevice physicalDevice);

protected:
    VulkanDevice* mDevice;
    bool          mRequirementsPassed;
};
