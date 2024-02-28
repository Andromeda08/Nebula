#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "Utility.hpp"

namespace Nebula::nvk
{
    class Instance
    {
    public:
        NVK_DISABLE_COPY(Instance);

        Instance(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

        const vk::Instance& handle() const { return m_instance; }

    private:
        vk::Instance                m_instance;
        std::vector<std::string>    m_layers;
        std::vector<std::string>    m_extensions;
    };
}