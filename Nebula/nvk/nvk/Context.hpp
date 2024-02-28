#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "IWindow.hpp"
#include "Command.hpp"
#include "Device.hpp"
#include "Utility.hpp"

namespace Nebula::nvk
{
    struct ContextCreateInfo
    {
        std::vector<const char*>    instance_extensions {};
        std::vector<const char*>    instance_layers {};
        bool                        ray_tracing_features {false};
        bool                        mesh_shader_features {false};
        bool                        debug_mode {false};
        bool                        validation_layers {false};
        std::shared_ptr<IWindow>    window;

        ContextCreateInfo() = default;

        inline ContextCreateInfo& set_instance_extensions(const std::vector<const char*>& value)
        {
            instance_extensions = value;
            return *this;
        }

        inline ContextCreateInfo& set_instance_layers(const std::vector<const char*>& value)
        {
            instance_layers = value;
            return *this;
        }

        inline ContextCreateInfo& set_ray_tracing_features(bool value)
        {
            ray_tracing_features = value;
            return *this;
        }

        inline ContextCreateInfo& set_mesh_shader_features(bool value)
        {
            mesh_shader_features = value;
            return *this;
        }

        inline ContextCreateInfo& set_debug_mode(bool value)
        {
            debug_mode = value;
            if (value)
            {
                instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
            return *this;
        }

        inline ContextCreateInfo& set_validation_layers(bool value)
        {
            validation_layers = value;
            if (value)
            {
                instance_layers.push_back("VK_LAYER_KHRONOS_validation");
            }
            return *this;
        }

        inline ContextCreateInfo& set_window(const std::shared_ptr<IWindow>& w)
        {
            window = w;

            for (const auto& ext : w->get_vulkan_extensions())
            {
                instance_extensions.push_back(ext);
            }

            return *this;
        }
    };

    class Instance;

    class Context
    {
    public:
        NVK_DISABLE_COPY(Context);

        explicit Context(const ContextCreateInfo& create_info);

        const std::shared_ptr<Instance>& instance() const { return m_instance; }

        const std::shared_ptr<Device>& device() const { return m_device; }

        const std::shared_ptr<CommandPool>& command_pool() const { return m_command_pool; }

        const vk::SurfaceKHR& surface() const { return m_surface; }

    private:
        void create_debug_messenger();

        std::shared_ptr<Instance>       m_instance;
        std::shared_ptr<Device>         m_device;
        std::shared_ptr<CommandPool>    m_command_pool;
        vk::SurfaceKHR                  m_surface;
        vk::DebugUtilsMessengerEXT      m_debug_messenger;
    };
}