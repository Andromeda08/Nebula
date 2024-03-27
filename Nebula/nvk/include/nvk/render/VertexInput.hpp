#pragma once

#include <concepts>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Nebula::nvk
{
    template <typename T>
    concept VertexInput = requires (T t, uint32_t u) {
        { T::attribute_descriptions(u, u) } -> std::same_as<std::vector<vk::VertexInputAttributeDescription>>;
        { T::binding_description(u) } -> std::same_as<vk::VertexInputBindingDescription>;
    };
}