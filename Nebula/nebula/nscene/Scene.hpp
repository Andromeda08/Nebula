#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <ncommon/Size2D.hpp>
#include <nmath/Utility.hpp>
#include <nscene/Camera.hpp>
#include <nscene/Light.hpp>
#include <nscene/Object.hpp>
#include <nscene/geometry/Geometry.hpp>
#include <nscene/geometry/Mesh.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Command.hpp>
#include <nvk/Device.hpp>
#include <wsi/Window.hpp>

namespace Nebula::ns
{
    class Scene
    {
    public:
        Scene(const Size2D& camera_size,
              const std::string& name,
              const std::shared_ptr<nvk::CommandPool>& command_pool,
              const std::shared_ptr<nvk::Device>& device,
              bool with_defaults = false);

        virtual ~Scene() = default;

        virtual void key_handler(const wsi::Window& window);

        virtual void mouse_handler(const wsi::Window& window);

        virtual void update(float dt, uint32_t current_frame);

        virtual void update(float dt, uint32_t current_frame, const vk::CommandBuffer& command_buffer);

        void next_camera();

        const std::shared_ptr<Camera>& active_camera() const
        {
            return m_cameras[m_active_camera];
        }

        const std::map<std::string, std::shared_ptr<Mesh>>& meshes()
        {
            return m_meshes;
        }

        const std::string& name() const
        {
            return m_name;
        }

        const std::vector<Object>& objects() const
        {
            return m_objects;
        }

        const std::vector<Light>& lights() const
        {
            return m_lights;
        }

        const std::shared_ptr<nvk::Buffer>& object_descriptions_buffer() const
        {
            return m_object_descriptions_buffer;
        }

        const std::shared_ptr<nvk::TLAS>& tlas() const
        {
            return m_top_level_as;
        }

    protected:
        void add_defaults();

        void create_camera_uniform_buffers();

        void create_object_description_buffers();

        void create_tlas();

        void update_tlas(const vk::CommandBuffer& command_buffer);

        std::vector<nvk::TLASInstanceInfo> collect_tlas_instances() const;

        uint32_t                                        m_active_camera {0};
        std::vector<std::shared_ptr<Camera>>            m_cameras;
        Size2D                                          m_camera_size;
        std::vector<Light>                              m_lights;
        std::map<std::string, std::shared_ptr<Mesh>>    m_meshes;
        std::vector<Object>                             m_objects;
        std::shared_ptr<nvk::Buffer>                    m_object_descriptions_buffer;
        std::shared_ptr<nvk::TLAS>                      m_top_level_as;

        std::array<std::shared_ptr<nvk::Buffer>, 2>     m_camera_uniform_buffer;

        const int32_t                                   m_id {nmath::rand()};
        const std::string                               m_name {"Unknown Scene"};
        std::shared_ptr<nvk::CommandPool>               m_command_pool;
        std::shared_ptr<nvk::Device>                    m_device;
    };
}