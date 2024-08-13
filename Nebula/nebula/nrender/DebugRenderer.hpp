#pragma once

#include <memory>
#include <vector>
#include <nscene/Object.hpp>
#include <nscene/Scene.hpp>
#include <nscene/geometry/Mesh.hpp>
#include <nvk/Barrier.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Command.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Device.hpp>
#include <nvk/Image.hpp>
#include <nvk/Swapchain.hpp>
#include <nvk/render/Framebuffer.hpp>
#include <nvk/render/Pipeline.hpp>
#include <nvk/render/RenderPass.hpp>
#include "Raytracer.hpp"

namespace Nebula::nrender
{
    struct DebugRenderPushConstant
    {
        glm::mat4 model {};
        glm::vec4 light_color { 1.0f, 1.0f, 1.0f, 0.75f };
    };

    class DebugRender
    {
    public:
        DebugRender(const std::shared_ptr<nvk::Device>&    device,
                    const std::shared_ptr<nvk::Swapchain>& swapchain,
                    const std::shared_ptr<ns::Scene>&      scene,
                    const std::shared_ptr<nvk::Image>&     target)
        : m_device(device)
        , m_scene(scene)
        , m_target(target)
        {
            using enum nvk::DescriptorType;
            using enum vk::ShaderStageFlagBits;

            auto descriptor_create_info = nvk::DescriptorCreateInfo()
                .add(eUniformBuffer, 0, eVertex)
                .set_count(swapchain->image_count())
                .set_name("Light Debug");
            m_descriptor = nvk::Descriptor::create(descriptor_create_info, m_device);

            auto render_pass_create_info = nvk::RenderPassCreateInfo()
                .add_color_attachment(m_target->properties().format,
                                      vk::ImageLayout::eColorAttachmentOptimal,
                                      vk::SampleCountFlagBits::e1,
                                      { 0.f, 0.f, 0.f, 0.f },
                                      vk::AttachmentLoadOp::eDontCare)
                .set_render_area({{0, 0}, swapchain->extent()})
                .set_name("Light Debug");
            m_render_pass = nvk::RenderPass::create(render_pass_create_info, m_device);

            auto framebuffer_create_info = nvk::FramebufferCreateInfo()
                .set_framebuffer_count(swapchain->image_count())
                .add_attachment(m_target->image_view())
                .set_render_pass(m_render_pass->render_pass())
                .set_extent(swapchain->extent())
                .set_name("Light Debug");
            m_framebuffers = nvk::Framebuffer::create(framebuffer_create_info, m_device);

            auto pipeline_create_info = nvk::PipelineCreateInfo()
                .set_pipeline_type(nvk::PipelineType::eGraphics)
                .add_push_constant({ eVertex, 0, sizeof(DebugRenderPushConstant) })
                .add_descriptor_set_layout(m_descriptor->layout())
                .add_shader("light_debug.vert.spv", eVertex)
                .add_shader("light_debug.frag.spv", eFragment)
                .add_attribute_descriptions<ns::Vertex>()
                .add_binding_description<ns::Vertex>()
                .set_cull_mode(vk::CullModeFlagBits::eNone)
                .set_sample_count(vk::SampleCountFlagBits::e1)
                .set_render_pass(m_render_pass->render_pass())
                .add_attachment(true)
                .set_name("Light Debug");
            m_pipeline = nvk::Pipeline::create(pipeline_create_info, m_device);

            auto uniform_data = m_scene->active_camera()->uniform_data();
            m_uniform_buffer.resize(swapchain->image_count());
            for (int32_t i = 0; i < m_uniform_buffer.size(); i++)
            {
                auto ub_create_info = nvk::BufferCreateInfo()
                    .set_buffer_type(nvk::BufferType::eUniform)
                    .set_name(fmt::format("Light Debug Camera #{}", i))
                    .set_size(sizeof(ns::CameraData));
                m_uniform_buffer[i] = nvk::Buffer::create(ub_create_info, m_device);
                m_uniform_buffer[i]->set_data(&uniform_data);
            }

            for (int32_t i = 0; i < m_descriptor->set_count(); i++)
            {
                vk::DescriptorBufferInfo buffer_info = { m_uniform_buffer[i]->buffer(), 0, sizeof(ns::CameraData)};

                auto write_info = nvk::DescriptorWriteInfo()
                    .set_set_index(i)
                    .add_uniform_buffer(0, buffer_info);
                m_descriptor->write(write_info);
            }

            m_sphere_mesh = m_scene->meshes().at("sphere");
        }

        void render(uint32_t current_frame, const vk::CommandBuffer& command_buffer, const RaytracerPushConstant& rt_light) const
        {
            DebugRenderPushConstant push_constant;

            auto uniform_data = m_scene->active_camera()->uniform_data();
            m_uniform_buffer[current_frame]->set_data(&uniform_data);

            if (rt_light.light_type != 0) return;

            m_render_pass->execute(command_buffer, m_framebuffers->get(current_frame), [&](const vk::CommandBuffer& cmd) {
                m_pipeline->bind(command_buffer);
                m_pipeline->bind_descriptor_set(command_buffer, m_descriptor->set(current_frame));

                // for (const auto& light : m_scene->lights()) {}

                push_constant = { glm::translate(glm::mat4(1.0f), glm::vec3(rt_light.light_position.x, rt_light.light_position.y, rt_light.light_position.z)), rt_light.light_intensity };
                m_pipeline->push_constants<DebugRenderPushConstant>(command_buffer, vk::ShaderStageFlagBits::eVertex, 0, &push_constant);
                m_sphere_mesh->draw(command_buffer);
            });
        }

        const std::shared_ptr<nvk::Image>& target() const { return m_target; }

    private:
        std::shared_ptr<ns::Scene>                m_scene;
        std::shared_ptr<ns::Mesh>                 m_sphere_mesh;

        std::shared_ptr<nvk::Image>               m_depth_buffer;
        std::shared_ptr<nvk::Image>               m_target;
        std::vector<std::shared_ptr<nvk::Buffer>> m_uniform_buffer;

        std::shared_ptr<nvk::Descriptor>          m_descriptor;
        std::shared_ptr<nvk::RenderPass>          m_render_pass;
        std::shared_ptr<nvk::Framebuffer>         m_framebuffers;
        std::shared_ptr<nvk::Pipeline>            m_pipeline;

        std::shared_ptr<nvk::Device>              m_device;
    };
}