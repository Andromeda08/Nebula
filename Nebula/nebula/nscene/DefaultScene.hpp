#pragma once

#include <format>
#include <random>
#include <nmath/Utility.hpp>
#include <nscene/Scene.hpp>
#include <nscene/geometry/primitives/Cube.hpp>
#include <nscene/geometry/primitives/Sphere.hpp>

namespace Nebula::ns
{
    class DefaultScene : public Scene
    {
    public:
        DefaultScene(const glm::ivec2& camera_size,
                     const std::shared_ptr<nvk::CommandPool>& command_pool,
                     const std::shared_ptr<nvk::Device>& device)
        : Scene(camera_size, "Default Scene", command_pool, device)
        {}

        ~DefaultScene() override = default;

    protected:
        void scene_init() override
        {
            // Default Camera
            auto camera = std::make_shared<Camera>(m_camera_size, glm::vec3(5, 5, 0));
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
            m_lights.push_back(light);

            Object plane {};
            plane.mesh = m_meshes["cube"];
            plane.name = std::format("Object {}", m_objects.size() + 1);
            plane.transform.scale = { 192.0f, 0.05f, 192.0f};
            m_objects.push_back(plane);

            Object plane1 {};
            plane1.mesh = m_meshes["cube"];
            plane1.name = std::format("Object {}", m_objects.size() + 1);
            plane1.transform.scale = { 192.0f, 0.05f, 192.0f};
            plane1.transform.translate = { 96.0f, 32.0f, 0.0f };
            plane1.transform.euler.z = glm::pi<float>() / 2.0f;
            plane1.rt_hit_group = 0;
            m_objects.push_back(plane1);

            Object plane2 {};
            plane2.mesh = m_meshes["cube"];
            plane2.name = std::format("Object {}", m_objects.size() + 1);
            plane2.transform.scale = { 192.0f, 0.05f, 192.0f};
            plane2.transform.translate = { 0.0f, 32.0f, -96.0f };
            plane2.transform.euler.x = glm::pi<float>() / 2.0f;
            plane2.rt_hit_group = 1;
            m_objects.push_back(plane2);

            Object plane3 {};
            plane3.mesh = m_meshes["cube"];
            plane3.name = std::format("Object {}", m_objects.size() + 1);
            plane3.transform.scale = { 192.0f, 0.05f, 192.0f};
            plane3.transform.translate = { 0.0f, 32.0f, 96.0f };
            plane3.transform.euler.x = glm::pi<float>() / 2.0f;
            plane3.rt_hit_group = 1;
            m_objects.push_back(plane3);

            std::vector<glm::vec4> color_pool = {
                {1.0f, 1.0f, 0.0f,  1.0f}, {0.0f, 0.8f, 1.0f,  1.0f},
                {1.0f, 0.0f, 0.25f, 1.0f}, {1.0f, 0.0f, 0.85f, 1.0f},
                {0.2f, 1.0f, 0.2f,  1.0f}, {0.1f, 0.8f, 0.9f,  1.0f},
                {1.0f, 0.1f, 0.1f,  1.0f}, {1.0f, 0.5f, 0.0f,  1.0f}
            };

            std::default_random_engine random;
            std::uniform_int_distribution<int32_t> dist(0, static_cast<int32_t>(color_pool.size() - 1));

            for (int32_t i = 0; i < 1024; i++)
            {
                nmath::Transform transform {
                    .translate = glm::vec3(nmath::randf(0.0f, 192.0f) - 96.0f + nmath::randf(),
                                           nmath::randf(-0.05f, 0.0f),
                                           nmath::randf(0.0f, 192.0f) - 96.0f + nmath::randf()),
                    .scale = glm::vec3(nmath::randf(1.0f, 6.0f),
                                       nmath::randf(1.0f, 18.0f),
                                       nmath::randf(1.0f, 8.0f)),
                    .euler = glm::vec3(nmath::randf(-30.0f, 30.0f),
                                       nmath::randf(-30.0f, 30.0f),
                                       nmath::randf(-30.0f, 30.0f)),
                };

                Object object {
                    .mesh         = m_meshes["cube"],
                    .name         = std::format("Object {}", m_objects.size() + 1),
                    .rt_hit_group = static_cast<uint32_t>(nmath::rand() % 3),
                    .solid_color  = color_pool[dist(random)],
                    .transform    = transform,
                };
                m_objects.push_back(object);
            }
        }
    };
}