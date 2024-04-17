#include "GBuffer.hpp"
#include <nrg/common/ResourceTraits.hpp>
#include <nrg/resource/Resources.hpp>
#include <nscene/Vertex.hpp>

namespace Nebula::nrg
{
    nrg_def_resource_requirements(GBuffer, ({
        std::make_shared<Requirement>(s_scene_data, ResourceUsage::eInput, ResourceType::eSceneData),
        std::make_shared<ImageRequirement>(s_position, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_normal, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_albedo, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_depth, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eD32Sfloat),
        std::make_shared<ImageRequirement>(s_motion_vec, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
    }))

    void GBuffer::initialize()
    {
        auto render_resolution = m_context->m_render_resolution.operator vk::Extent2D();

        auto scene = get_resource<SceneResource>(s_scene_data).get_scene();
        auto position = get_resource<ImageResource>(s_position).get_image();
        auto normal = get_resource<ImageResource>(s_normal).get_image();
        auto albedo = get_resource<ImageResource>(s_albedo).get_image();
        auto depth = get_resource<ImageResource>(s_depth).get_image();
        auto motion_vec = get_resource<ImageResource>(s_motion_vec).get_image();

        auto descriptor_create_info = nvk::DescriptorCreateInfo()
            .add(Nebula::nvk::DescriptorType::eUniformBuffer, 0, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
            .set_count(2)
            .set_name("G-Buffer");
        m_descriptor = std::make_shared<nvk::Descriptor>(descriptor_create_info, m_device);

        auto render_pass_create_info = nvk::RenderPassCreateInfo()
            .add_attachment(position)
            .add_attachment(normal)
            .add_attachment(albedo)
            .add_attachment(motion_vec)
            .set_depth_attachment(depth)
            .set_name("G-Buffer RenderPass")
            .set_render_area({{0,0}, render_resolution});
        m_render_pass = std::make_shared<nvk::RenderPass>(render_pass_create_info, m_device);

        auto framebuffer_create_info = nvk::FramebufferCreateInfo()
            .set_framebuffer_count(m_context->m_frames)
            .set_render_pass(m_render_pass->render_pass())
            .set_extent(render_resolution)
            .set_name("G-Buffer Framebuffer")
            .add_attachment(position->image_view())
            .add_attachment(normal->image_view())
            .add_attachment(albedo->image_view())
            .add_attachment(motion_vec->image_view())
            .add_attachment(depth->image_view());
        m_framebuffers = std::make_shared<nvk::Framebuffer>(framebuffer_create_info, m_device);

        auto pipeline_create_info = nvk::PipelineCreateInfo()
            .set_pipeline_type(nvk::PipelineType::eGraphics)
            .add_push_constant({ vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, sizeof(ns::ObjectPushConstant) })
            .add_descriptor_set_layout(m_descriptor->layout())
            .add_attribute_descriptions<ns::Vertex>()
            .add_binding_description<ns::Vertex>()
            .add_shader("nrg_g_buffer.vert.spv", vk::ShaderStageFlagBits::eVertex)
            .add_shader("nrg_g_buffer.frag.spv", vk::ShaderStageFlagBits::eFragment)
            .set_attachment_count(4)
            .set_sample_count(vk::SampleCountFlagBits::e1)
            .set_render_pass(m_render_pass->render_pass())
            .set_name("G-Buffer");
        m_pipeline = std::make_shared<nvk::Pipeline>(pipeline_create_info, m_device);

        m_uniform_buffer.resize(m_context->m_frames);
        for (int32_t i = 0; i < m_context->m_frames; i++)
        {
            nvk::BufferCreateInfo buf_create_info{};
            buf_create_info
                .set_buffer_type(nvk::BufferType::eUniform)
                .set_name(std::format("G-Buffer Uniform #{}", i))
                .set_size(sizeof(ns::CameraData));

            m_uniform_buffer[i] = std::make_shared<nvk::Buffer>(buf_create_info, m_device);

            vk::DescriptorBufferInfo buffer_info = { m_uniform_buffer[i]->buffer(), 0, sizeof(ns::CameraData)};
            auto write_info = nvk::DescriptorWriteInfo()
                .set_set_index(i)
                .add_uniform_buffer(0, buffer_info);
            m_descriptor->write(write_info);
        }
    }

    void GBuffer::execute(const vk::CommandBuffer& command_buffer)
    {
        auto scene = get_resource<SceneResource>(s_scene_data).get_scene();
        m_render_pass->execute(command_buffer, m_framebuffers->get(m_current_frame), [&](const vk::CommandBuffer& cmd) {
            m_pipeline->bind(cmd);
            cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline->layout(), 0, 1, &m_descriptor->set(m_current_frame), 0, nullptr);
            for (const auto& object : scene->objects())
            {
                using enum vk::ShaderStageFlagBits;
                auto push_constant = object.get_push_constants();
                cmd.pushConstants(m_pipeline->layout(), eVertex | eFragment, 0, sizeof(ns::ObjectPushConstant), &push_constant);
                object.mesh->draw(cmd);
            }
        });
    }

    void GBuffer::update()
    {
        auto scene = get_resource<SceneResource>(s_scene_data).get_scene();
        auto uniform_data = scene->active_camera()->uniform_data();
        m_uniform_buffer[m_current_frame]->set_data(&uniform_data);
    }
}