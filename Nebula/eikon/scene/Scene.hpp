#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <tiny_gltf.h>
#include <fmt/printf.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nvk/Barrier.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Device.hpp>
#include <nvk/Image.hpp>

namespace Eikon
{
    using namespace Nebula;

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 color;
    };

    struct Material
    {
        glm::vec4   base_color_factor = glm::vec4(1.0f);
        uint32_t    base_color_texture_index;
        std::string name;
    };

    struct Texture
    {
        int32_t     image_index;
        std::string name;
    };

    struct Primitive
    {
        uint32_t first_index;
        uint32_t index_count;
        int32_t  material_index;
    };

    struct Mesh
    {
        std::vector<Primitive> primitives;
    };

    struct Node
    {
        std::shared_ptr<Node>              parent = nullptr;
        glm::mat4                          matrix = glm::mat4(1.0f);
        Mesh                               mesh;
        std::vector<std::shared_ptr<Node>> children;
        std::string                        name;
    };

    class glTFScene
    {
    public:
        explicit glTFScene(const std::string& gltf_file, const std::shared_ptr<nvk::Device>& device, const std::shared_ptr<nvk::CommandPool>& command)
        : m_device(device), m_command_pool(command)
        {
            using clock = std::chrono::high_resolution_clock;
            const auto start = clock::now();
            load_gltf(gltf_file);

            load_images();
            load_textures();
            load_materials();

            std::vector<uint32_t> index_data;
            std::vector<Vertex>   vertex_data;

            const auto& scene = m_model.scenes[0];
            for (size_t i = 0; i < scene.nodes.size(); i++)
            {
                const auto node = m_model.nodes[scene.nodes[i]];
                load_node(node, nullptr, index_data, vertex_data);
            }

            const auto end = clock::now();
            const auto time = duration_cast<std::chrono::seconds>(end - start);

            fmt::println("[{}s] Loaded {} images, {} textures, {} materials and {} nodes with a total of {} indices and {} vertices",
                time.count(), m_images.size(), m_textures.size(), m_materials.size(), count_nodes(m_nodes[0]), index_data.size(), vertex_data.size());

            const auto vb_create_info = nvk::BufferCreateInfo()
                .set_buffer_type(nvk::BufferType::eVertex)
                .set_name(gltf_file)
                .set_size(sizeof(Vertex) * vertex_data.size());
            m_vertex_buffer = nvk::Buffer::create_with_data(vb_create_info, vertex_data.data(), m_device, m_command_pool);

            const auto ix_create_info = nvk::BufferCreateInfo()
                .set_buffer_type(nvk::BufferType::eIndex)
                .set_name(gltf_file)
                .set_size(sizeof(uint32_t) * index_data.size());
            m_index_buffer = nvk::Buffer::create_with_data(ix_create_info, index_data.data(), m_device, m_command_pool);
        }

    private:
        void load_gltf(const std::string& gltf_file)
        {
            fmt::println("Loading scene: {}", gltf_file);

            tinygltf::TinyGLTF loader;
            std::string err, warn;
            bool result = loader.LoadBinaryFromFile(&m_model, &err, &warn, gltf_file);

            if (!err.empty()) fmt::println("Scene \"{}\" loaded with error(s): {}", gltf_file, err);
            if (!warn.empty()) fmt::println("Scene \"{}\" loaded with warning(s): {}", gltf_file, warn);
            if (!result)
            {
                fmt::println("Failed to parse GLTF file for scene: {}", gltf_file);
            }
        }

        void load_images()
        {
            m_images.resize(m_model.images.size());
            for (size_t i = 0; i < m_model.images.size(); i++)
            {
                auto& gltf_image = m_model.images[i];

                unsigned char* buffer = nullptr;
                vk::DeviceSize buffer_size = 0;

                if (gltf_image.component == 3)
                {
                    buffer_size = gltf_image.width * gltf_image.height * 4;
                    buffer = new unsigned char[buffer_size];
                    unsigned char* rgba = buffer;
                    unsigned char* rgb = &gltf_image.image[0];
                    for (size_t j = 0; i < gltf_image.width * gltf_image.height; ++j)
                    {
                        std::memcpy(rgba, rgb, sizeof(unsigned char) * 3);
                        rgba += 4;
                        rgb += 3;
                    }
                }
                else
                {
                    buffer = &gltf_image.image[0];
                    buffer_size = gltf_image.image.size();
                }

                auto buffer_info = nvk::BufferCreateInfo()
                    .set_size(buffer_size)
                    .set_buffer_type(nvk::BufferType::eStaging);
                auto staging_buffer = std::make_unique<nvk::Buffer>(buffer_info, m_device);
                staging_buffer->set_data(buffer);

                nvk::ImageCreateInfo image_info = nvk::ImageCreateInfo()
                    .set_aspect_flags(vk::ImageAspectFlagBits::eColor)
                    .set_extent({ static_cast<uint32_t>(gltf_image.width), static_cast<uint32_t>(gltf_image.height) })
                    .set_format(vk::Format::eR8G8B8A8Unorm)
                    .set_sample_count(vk::SampleCountFlagBits::e1)
                    .set_usage_flags(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst)
                    .set_tiling(vk::ImageTiling::eOptimal)
                    .set_name(gltf_image.name)
                    .set_with_sampler(true);

                std::shared_ptr<nvk::Image> image = std::make_shared<nvk::Image>(image_info, m_device);

                m_command_pool->exec_single_time_command([&](const vk::CommandBuffer& command_buffer) {
                    nvk::ImageBarrier(image, image->state().layout, vk::ImageLayout::eTransferDstOptimal);

                    staging_buffer->copy_to_image(*image, command_buffer);

                    nvk::ImageBarrier(image, image->state().layout, vk::ImageLayout::eShaderReadOnlyOptimal);
                });

                m_images.push_back(image);

                fmt::println("- Loaded image ({}/{}): {}", i + 1, m_model.images.size(), gltf_image.name);
            }
        }

        void load_textures()
        {
            m_textures.resize(m_model.textures.size());
            for (size_t i = 0; i < m_model.textures.size(); i++)
            {
                const auto& texture = m_model.textures[i];
                m_textures[i].image_index = texture.source;
                m_textures[i].name = texture.name;
            }

        }

        void load_materials()
        {
            m_materials.resize(m_model.materials.size());
            for (size_t i = 0; i < m_model.materials.size(); i++)
            {
                auto gltf_material = m_model.materials[i];
                if (gltf_material.values.contains("baseColorFactor"))
                {
                    m_materials[i].base_color_factor = glm::make_vec4(gltf_material.values["baseColorFactor"].ColorFactor().data());
                }
                if (gltf_material.values.contains("baseColorTexture"))
                {
                    m_materials[i].base_color_texture_index = gltf_material.values["baseColorTexture"].TextureIndex();
                }
                m_materials[i].name = gltf_material.name;
            }
        }

        void load_node(const tinygltf::Node& input, const std::shared_ptr<Node>& parent, std::vector<uint32_t>& indices, std::vector<Vertex>& vertices)
        {
            auto node = std::make_shared<Node>();
            node->matrix = glm::mat4(1.0f);
            node->parent = parent;

            if (input.translation.size() == 3)
            {
                node->matrix = glm::translate(node->matrix, glm::vec3(glm::make_vec3(input.translation.data())));
            }
            if (input.rotation.size() == 4)
            {
                const glm::quat q = glm::make_quat(input.rotation.data());
                node->matrix *= glm::mat4(q);
            }
            if (input.scale.size() == 3)
            {
                node->matrix = glm::scale(node->matrix, glm::vec3(glm::make_vec3(input.scale.data())));
            }
            if (input.matrix.size() == 16)
            {
                node->matrix = glm::make_mat4x4(input.matrix.data());
            }

            if (input.children.size() > 0)
            {
                for (size_t i = 0; i < input.children.size(); i++)
                {
                    load_node(m_model.nodes[input.children[i]], node, indices, vertices);
                }
            }

            if (input.mesh > -1)
            {
                const tinygltf::Mesh mesh = m_model.meshes[input.mesh];
                for (size_t i = 0; i < mesh.primitives.size(); i++)
                {
                    const auto& gltf_primitive = mesh.primitives[i];

                    uint32_t first_index = static_cast<uint32_t>(indices.size());
                    uint32_t vertex_start = static_cast<uint32_t>(vertices.size());
                    uint32_t index_count = 0;

                    const float* position_buffer = nullptr;
                    const float* normal_buffer = nullptr;
                    const float* uv_buffer = nullptr;
                    uint32_t vertex_count = 0;

                    if (gltf_primitive.attributes.contains("POSITION"))
                    {
                        const auto& accessor = m_model.accessors[gltf_primitive.attributes.at("POSITION")];
                        const auto& view = m_model.bufferViews[accessor.bufferView];
                        position_buffer = reinterpret_cast<const float*>(&(m_model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                        vertex_count = accessor.count;
                    }
                    if (gltf_primitive.attributes.contains("NORMAL"))
                    {
                        const auto& accessor = m_model.accessors[gltf_primitive.attributes.at("NORMAL")];
                        const auto& view = m_model.bufferViews[accessor.bufferView];
                        normal_buffer = reinterpret_cast<const float*>(&(m_model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    }
                    if (gltf_primitive.attributes.contains("TEXCOORD_0"))
                    {
                        const auto& accessor = m_model.accessors[gltf_primitive.attributes.at("TEXCOORD_0")];
                        const auto& view = m_model.bufferViews[accessor.bufferView];
                        uv_buffer = reinterpret_cast<const float*>(&(m_model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    }

                    for (size_t v = 0; v < vertex_count; v++)
                    {
                        Vertex vtx {
                            .position = glm::vec4(glm::make_vec3(&position_buffer[v * 3]), 1.0f),
                            .normal = glm::normalize(glm::vec3(normal_buffer ? glm::make_vec3(&normal_buffer[v * 3]) : glm::vec3(0.0f))),
                            .uv = uv_buffer ? glm::make_vec2(&uv_buffer[v * 2]) : glm::vec2(0.0f),
                            .color = glm::vec3(1.0f),
                        };
                        vertices.push_back(vtx);
                    }

                    const auto& accessor = m_model.accessors[gltf_primitive.indices];
                    const auto& view = m_model.bufferViews[accessor.bufferView];
                    const auto& buffer = m_model.buffers[view.buffer];

                    index_count += static_cast<uint32_t>(accessor.count);

                    switch (accessor.componentType)
                    {
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:{
                            const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
                            for (size_t j = 0; j < accessor.count; j++)
                            {
                                indices.push_back(buf[j] + vertex_start);
                            }
                            break;
                        }
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:{
                            const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
                            for (size_t j = 0; j < accessor.count; j++)
                            {
                                indices.push_back(buf[j] + vertex_start);
                            }
                            break;
                        }
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:{
                            const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + view.byteOffset]);
                            for (size_t j = 0; j < accessor.count; j++)
                            {
                                indices.push_back(buf[j] + vertex_start);
                            }
                            break;
                        }
                        default: {
                            fmt::println("Index component type {} not supported", accessor.componentType);
                            return;
                        }
                    }

                    Primitive primitive {
                        .first_index = first_index,
                        .index_count = index_count,
                        .material_index = gltf_primitive.material,
                    };
                    node->mesh.primitives.push_back(primitive);
                }
            }

            if (parent)
            {
                parent->children.push_back(node);
            }
            else
            {
                m_nodes.push_back(node);
            }
        }

        static uint32_t count_nodes(const std::shared_ptr<Node>& root_node)
        {
            if (root_node == nullptr) return 0;
            uint32_t count = 1;
            for (const auto& child : root_node->children)
            {
                count += count_nodes(child);
            }
            return count;
        }

        std::shared_ptr<nvk::Device>      m_device;
        std::shared_ptr<nvk::CommandPool> m_command_pool;

        tinygltf::Model                    m_model;

        std::vector<Texture>               m_textures;
        std::vector<Material>              m_materials;
        std::vector<std::shared_ptr<Node>> m_nodes;

        std::shared_ptr<nvk::Buffer>       m_index_buffer;
        std::shared_ptr<nvk::Buffer>       m_vertex_buffer;

        std::vector<std::shared_ptr<nvk::Image>> m_images;
    };
}
