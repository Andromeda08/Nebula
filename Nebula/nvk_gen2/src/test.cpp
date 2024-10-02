#include "VulkanRHI.hpp"
#include "VulkanExtension.hpp"
#include <iostream>

std::vector<std::string> colors = {
    "\x1b[31m", "\x1b[32m", "\x1b[33m", "\x1b[34m", "\x1b[35m", "\x1b[36m",
};

void log_device_ext_chain(vk::DeviceCreateInfo& ci)
{
    std::cout << "[DeviceCreateInfo > pNext]" << std::endl;
    void* current = (void*) ci.pNext;
    int32_t count = 0;
    while (current != nullptr)
    {
        std::string name;
        auto* someStruct = reinterpret_cast<vk::PhysicalDeviceAccelerationStructureFeaturesKHR*>(current);
        std::cout << std::format("{}{}\\ {}", colors[count % colors.size()], std::string(count, ' '),to_string(someStruct->sType)) << std::endl;
        current = someStruct->pNext;
        count++;
    }
    std::cout << "\x1b[0m";
}

int main()
{
    vk::DeviceCreateInfo createInfo;
    auto deviceExtensions = VulkanDeviceExtension::getRHIDeviceExtensions(nullptr);

    for (auto& ext : deviceExtensions)
    {
        ext->setSupported();
        ext->postSupportCheck();
        ext->preCreateDevice(createInfo);

        std::cout << std::format("{} : {}", ext->extensionName(), ext->isEnabled() ? "Enabled" : ext->isSupported() ? "Disabled (Supported)" : "Unavailable") << std::endl;
    }

    log_device_ext_chain(createInfo);

    auto RHI = std::make_unique<VulkanRHI>();

    return 0;
}