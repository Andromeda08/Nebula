#include "Scene.hpp"
#include <format>
#include <random>
#include <nmath/Utility.hpp>
#include <nscene/geometry/primitives/Cube.hpp>
#include <nscene/geometry/primitives/Sphere.hpp>

namespace Nebula::ns
{
    Scene::Scene(const Size2D& camera_size,
                 const std::string& name,
                 const std::shared_ptr<nvk::CommandPool>& command_pool,
                 const std::shared_ptr<nvk::Device>& device,
                 bool with_defaults)
    : m_camera_size(camera_size), m_name(name), m_command_pool(command_pool), m_device(device)
    {
        if (with_defaults)
        {
            add_defaults();
        }

        create_object_description_buffers();

        if (m_device->is_raytracing_enabled())
        {
            // Create TLAS
        }
    }

    void Scene::update(float dt)
    {

    }

    void Scene::add_defaults()
    {
        // Default Camera
        auto camera = std::make_shared<Camera>(glm::ivec2(m_camera_size.width, m_camera_size.height),
                                               glm::vec3(5, 5, 0));
        m_cameras.push_back(camera);

        // Cube and Sphere primitive meshes
        auto cube_mesh_create_info = MeshCreateInfo()
            .set_p_geometry(new Cube())
            .set_name("cube");
        m_meshes["cube"] = std::make_shared<Mesh>(cube_mesh_create_info, m_device, m_command_pool);

        auto sphere_mesh_create_info = MeshCreateInfo()
            .set_p_geometry(new Sphere())
            .set_name("sphere");
        m_meshes["sphere"] = std::make_shared<Mesh>(sphere_mesh_create_info, m_device, m_command_pool);

        // Default lights
        Light light {
            .position = glm::vec4(-12.0f, 10, 5.0f, 1.0f),
            .color = glm::vec4(1.0f),
        };

        // Default objects [n = 1024 + 1]
        Object plane {};
        plane.mesh = m_meshes["cube"];
        plane.name = std::format("Object {}", m_objects.size() + 1);
        plane.transform.scale = { 192.0f, 0.05f, 192.0f};

        std::vector<glm::vec4> color_pool = {
            {1.0f, 1.0f, 0.0f,  1.0f}, {0.0f, 0.8f, 1.0f,  1.0f},
            {1.0f, 0.0f, 0.25f, 1.0f}, {1.0f, 0.0f, 0.85f, 1.0f},
            {0.2f, 1.0f, 0.2f,  1.0f}, {0.1f, 0.8f, 0.9f,  1.0f},
            {1.0f, 0.1f, 0.1f,  1.0f}, {1.0f, 0.5f, 0.0f,  1.0f}
        };

        // std::random_device r;
        std::default_random_engine random;
        std::uniform_int_distribution<int32_t> dist(0, static_cast<int32_t>(color_pool.size() - 1));

        for (int32_t i = 0; i < 1024; i++)
        {
            nmath::Transform transform {
                .translate = glm::vec3(nmath::randf(0.0f, 192.0f) - 96.0f + nmath::randf(),
                                       nmath::randf(-0.05f, 0.0f),
                                       nmath::randf(0.0f, 192.0f) - 96.0f + nmath::randf()),
                .scale = glm::vec3(nmath::randf(1.0f, 4.0f),
                                   nmath::randf(1.0f, 16.0f),
                                   nmath::randf(1.0f, 5.0f)),
                .euler = glm::vec3(nmath::randf(-30.0f, 30.0f),
                                   nmath::randf(-30.0f, 30.0f),
                                   nmath::randf(-30.0f, 30.0f)),
            };

            Object object {
                .mesh         = m_meshes["cube"],
                .name         = std::format("Object {}", m_objects.size() + 1),
                .rt_hit_group = 0,
                .solid_color  = color_pool[dist(random)],
                .transform    = transform,
            };
            m_objects.push_back(object);
        }
    }

    void Scene::key_handler(const wsi::Window& window)
    {
        active_camera()->register_keys(window.handle());
    }

    void Scene::mouse_handler(const wsi::Window& window)
    {
        active_camera()->register_mouse(window.handle());
    }

    void Scene::next_camera()
    {
        m_active_camera = (m_active_camera + 1) % m_cameras.size();
    }

    void Scene::create_object_description_buffers()
    {
        std::vector<ObjectDescription> obj_descriptions;
        for (const auto& obj : m_objects)
        {
            obj_descriptions.push_back(obj.get_description());
        }

        auto name = std::format("[{} Scene] Object Descriptions", m_name);
        auto od_create_info = nvk::BufferCreateInfo()
            .set_buffer_type(nvk::BufferType::eStorage)
            .set_name(name)
            .set_size(sizeof(ObjectDescription) * obj_descriptions.size());
        m_object_descriptions_buffer = std::make_shared<nvk::Buffer>(od_create_info, m_device);

        auto odstgci = nvk::BufferCreateInfo()
            .set_buffer_type(nvk::BufferType::eStaging)
            .set_name(name)
            .set_size(od_create_info.size);
        auto od_staging = std::make_shared<nvk::Buffer>(odstgci, m_device);
        od_staging->set_data(obj_descriptions.data());

        m_command_pool->exec_single_time_command([&](const vk::CommandBuffer& command_buffer){
            od_staging->copy_to_buffer(*m_object_descriptions_buffer, command_buffer);
        });
    }
}