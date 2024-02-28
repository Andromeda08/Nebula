#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace Nebula::ns
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;

        static std::vector<vk::VertexInputAttributeDescription>
        attribute_descriptions(uint32_t base_location = 0, uint32_t binding = 0)
        {
            return {
                { base_location + 0, binding, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position) },
                { base_location + 1, binding, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal) },
                { base_location + 2, binding, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv) },
            };
        }

        static vk::VertexInputBindingDescription binding_description(uint32_t binding = 0)
        {
            return {
                binding, sizeof(Vertex), vk::VertexInputRate::eVertex
            };
        }
    };
}