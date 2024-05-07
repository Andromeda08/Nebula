#include "Scene.hpp"
#include <format>

namespace Nebula::ns
{
    Scene::Scene(const glm::ivec2& camera_size,
                 const std::string& name,
                 const std::shared_ptr<nvk::CommandPool>& command_pool,
                 const std::shared_ptr<nvk::Device>& device)
    : m_camera_size(camera_size), m_name(name), m_command_pool(command_pool), m_device(device)
    {
    }

    void Scene::init()
    {
        scene_init();

        if (!m_objects.empty())
        {
            create_object_description_buffers();
        }

        if (m_device->is_raytracing_enabled())
        {
            create_tlas();
        }

        create_camera_uniform_buffers();
    }

    void Scene::update(float dt, uint32_t current_frame)
    {
        auto uniform_data = active_camera()->uniform_data();
        m_camera_uniform_buffer[current_frame]->set_data(&uniform_data);
    }

    void Scene::update(float dt, uint32_t current_frame, const vk::CommandBuffer& command_buffer)
    {
        update_tlas(command_buffer);
        update(dt, current_frame);
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

    std::vector<nvk::TLASInstanceInfo> Scene::collect_tlas_instances() const
    {
        std::vector<nvk::TLASInstanceInfo> result;
        for (const auto& object : m_objects)
        {
            result.push_back(object.get_tlas_instance_info());
        }
        return result;
    }

    void Scene::create_tlas()
    {
        if (!m_device->is_raytracing_enabled()) return;
        auto instances = collect_tlas_instances();
        m_top_level_as = nvk::TLAS::create({ instances, m_name }, m_device, m_command_pool);
    }

    void Scene::update_tlas(const vk::CommandBuffer& command_buffer)
    {
        if (!m_device->is_raytracing_enabled()) return;
        auto instances = collect_tlas_instances();
        auto update_info = nvk::TLASUpdateInfo { instances };
        m_top_level_as->update(update_info, command_buffer);
    }

    void Scene::create_camera_uniform_buffers()
    {
        for (int32_t i = 0; i < m_camera_uniform_buffer.size(); i++)
        {
            auto ub_create_info = nvk::BufferCreateInfo()
                .set_buffer_type(nvk::BufferType::eUniform)
                .set_name(std::format("{} Scene Camera UB #{}", m_name, i))
                .set_size(sizeof(ns::CameraData));
            m_camera_uniform_buffer[i] = nvk::Buffer::create(ub_create_info, m_device);

            if (active_camera())
            {
                auto uniform_data = active_camera()->uniform_data();
                m_camera_uniform_buffer[i]->set_data(&uniform_data);
            }
        }
    }
}