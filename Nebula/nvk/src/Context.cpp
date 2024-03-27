#include "Context.hpp"
#include <iostream>
#include <nlog/nlog.hpp>
#include "DeviceExtensions.hpp"
#include "Instance.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace Nebula::nvk
{
    nvk::Context::Context(const ContextCreateInfo& create_info)
    {
        vk::DynamicLoader dl;
        auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        m_instance = std::make_shared<Instance>(create_info.instance_extensions, create_info.instance_layers);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance->handle());

        if (create_info.debug_mode)
        {
            create_debug_messenger();
        }

        create_info.window->create_surface(m_instance->handle(), &m_surface);

        auto device_extensions = DeviceExtensions()
            .set_ray_tracing_features(create_info.ray_tracing_features)
            .set_mesh_shading_features(create_info.mesh_shader_features);

        m_device = std::make_shared<Device>(m_instance->handle(), device_extensions);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_device->handle());

        m_command_pool = std::make_shared<CommandPool>(m_device);
    }

    void Context::create_debug_messenger()
    {
        auto severity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;

        auto type = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

        vk::DebugUtilsMessengerCreateInfoEXT create_info;
        create_info
            .setMessageSeverity(severity)
            .setMessageType(type)
            .setPfnUserCallback(Utility::debug_callback);

        if (const vk::Result result = m_instance->handle().createDebugUtilsMessengerEXT(&create_info, nullptr, &m_debug_messenger);
            result != vk::Result::eSuccess)
        {
            std::cerr << nlog::fmt_error("Failed to setup {} ({})", nlog::cyan("vk::DebugUtilsMessengerEXT"), to_string(result)) << std::endl;
        }

        #ifdef NVK_VERBOSE
        std::cout << nlog::fmt_info("Created {}", nlog::cyan("vk::DebugUtilsMessengerEXT")) << std::endl;
        #endif
    }
}