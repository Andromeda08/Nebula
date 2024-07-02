#include "Instance.hpp"
#include <set>
#include <sstream>
#include <stdexcept>
#include "Utilities.hpp"

namespace Nebula::nvk
{
    Instance::Instance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers)
    {
        std::vector<vk::LayerProperties> available_layers = vk::enumerateInstanceLayerProperties();
        std::set<std::string> requested_layers;
        std::vector<const char*> supported_layers;

        for (const auto& layer: layers) {
            requested_layers.insert(std::string(layer));
        }
        for (const auto& layer : available_layers)
        {
            if (requested_layers.contains(layer.layerName))
            {
                supported_layers.push_back(layer.layerName);
            }
        }

        std::vector<vk::ExtensionProperties> available_extensions = vk::enumerateInstanceExtensionProperties();
        std::set<std::string>                requested_extensions;
        std::vector<const char*>             supported_extensions;

        for (const auto& extension : extensions)
        {
            requested_extensions.insert(std::string(extension));
        }
        for (const auto& extension : available_extensions)
        {
            if (requested_extensions.contains(extension.extensionName))
            {
                supported_extensions.push_back(extension.extensionName);
            }
        }

        vk::ApplicationInfo app_info;
        app_info.setApiVersion(VK_API_VERSION_1_3);

        vk::InstanceCreateInfo create_info;
        create_info
            .setPApplicationInfo(&app_info)
            .setEnabledLayerCount(supported_layers.size())
            .setPpEnabledLayerNames(supported_layers.data())
            .setEnabledExtensionCount(supported_extensions.size())
            .setPpEnabledExtensionNames(supported_extensions.data());

        if (const vk::Result result = vk::createInstance(&create_info, nullptr, &m_instance);
            result != vk::Result::eSuccess)
        {
            throw make_exception("Failed to create {} ({})", Format::cyan("vk::Instance"), to_string(result));
        }

        for (const auto& layer : supported_layers)
        {
            m_layers.emplace_back(layer);
        }
        for (const auto& extension : supported_extensions)
        {
            m_extensions.emplace_back(extension);
        }

        if (NVK_LOG_LEVEL >= NVK_LOG_LEVEL_VERBOSE)
        {
            std::stringstream layers_ss;
            for (const auto& layer : m_layers)
            {
                layers_ss << layer << ", ";
            }

            std::stringstream exts_ss;
            for (const auto& ext : m_extensions)
            {
                exts_ss << ext << ", ";
            }

            print_verbose("Created {} with {}/{} layers and {}/{} extensions\n\tLayers: {}\n\tExtensions: {}",
                          Format::cyan("vk::Instance"), m_layers.size(), requested_layers.size(),
                          m_extensions.size(), requested_extensions.size(),
                          layers_ss.str(), exts_ss.str());
        }
    }
}