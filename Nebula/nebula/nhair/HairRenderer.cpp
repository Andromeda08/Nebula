#include "HairRenderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.hpp>
#include <nlog/nlog.hpp>

namespace Nebula::nhair
{
    HairRenderer::HairRenderer(const std::shared_ptr<nvk::Device>& device,
                               const std::shared_ptr<nvk::Swapchain>& swapchain)
    {
        // Mesh Shading Pipeline
        #pragma region
        {
            m_render_res = swapchain->extent();
            m_clear_values[0].setColor(std::array{0.0f, 0.0f, 0.0f, 1.0f});
            m_clear_values[1].setDepthStencil({1.0f, 0});

            auto depth_info = nvk::ImageCreateInfo()
                .set_aspect_flags(vk::ImageAspectFlagBits::eDepth)
                .set_extent(swapchain->extent())
                .set_format(vk::Format::eD32Sfloat)
                .set_usage_flags(vk::ImageUsageFlagBits::eDepthStencilAttachment)
                .set_name("Hair Depth Buffer");
            m_depth = std::make_shared<nvk::Image>(depth_info, device);

            m_render_pass = nvk::RenderPass::Builder()
                .add_color_attachment(swapchain->format(), vk::ImageLayout::ePresentSrcKHR)
                .set_depth_attachment(m_depth->properties().format)
                .make_subpass()
                .with_name("Hair")
                .create(device);

            auto descriptor_create_info = nvk::DescriptorCreateInfo()
                .add(Nebula::nvk::DescriptorType::eUniformBuffer, 0,
                     vk::ShaderStageFlagBits::eMeshEXT | vk::ShaderStageFlagBits::eFragment)
                .set_count(2)
                .set_name("Camera")
                .enable_ring_mode();
            m_descriptor = std::make_shared<nvk::Descriptor>(descriptor_create_info, device);

            nvk::PipelineCreateInfo pipeline_create_info;
            pipeline_create_info
                .add_push_constant(
                    {vk::ShaderStageFlagBits::eTaskEXT | vk::ShaderStageFlagBits::eMeshEXT, 0, sizeof(HairConstants)})
                .add_descriptor_set_layout(m_descriptor->layout())
                .set_pipeline_type(nvk::PipelineType::eGraphics)
                .add_shader("hair.task.glsl.spv", vk::ShaderStageFlagBits::eTaskEXT)
                .add_shader("hair.mesh.glsl.spv", vk::ShaderStageFlagBits::eMeshEXT)
                .add_shader("hair.frag.spv", vk::ShaderStageFlagBits::eFragment)
                .set_attachment_count(1)
                .set_cull_mode(vk::CullModeFlagBits::eNone)
                .set_render_pass(m_render_pass)
                .set_name("Hair");
            m_pipeline = std::make_shared<nvk::Pipeline>(pipeline_create_info, device);

            auto fbci = vk::FramebufferCreateInfo()
                .setRenderPass(m_render_pass)
                .setHeight(swapchain->extent().height)
                .setWidth(swapchain->extent().width)
                .setLayers(1)
                .setAttachmentCount(2);
            std::vector<vk::ImageView> attachments{swapchain->image_view(0), m_depth->image_view()};
            for (int32_t i = 0; i < m_framebuffers.size(); i++) {
                attachments[0] = swapchain->image_view(i);
                fbci.setPAttachments(attachments.data());

                if (vk::Result result = device->handle().createFramebuffer(&fbci, nullptr, &m_framebuffers[i]);
                    result != vk::Result::eSuccess) {
                    throw nlog::make_exception("Framebuffer creation failed.");
                }

                device->name_object(std::format("Hair Framebuffer {}", i),
                                    (uint64_t) m_framebuffers[i].operator VkFramebuffer(),
                                    vk::ObjectType::eFramebuffer);
            }

            m_uniform_buffers.resize(2);
            for (int32_t i = 0; i < 2; i++) {
                nvk::BufferCreateInfo buf_create_info{};
                buf_create_info
                    .set_buffer_type(nvk::BufferType::eUniform)
                    .set_name(std::format("Camera #{}", i))
                    .set_size(sizeof(ns::CameraData));

                m_uniform_buffers[i] = std::make_shared<nvk::Buffer>(buf_create_info, device);
            }
        }
        #pragma endregion

        // Compute Pipeline
        {
            auto descriptor_create_info = nvk::DescriptorCreateInfo()
                .add(nvk::DescriptorType::eStorageBuffer, 0, vk::ShaderStageFlagBits::eCompute)
                .add(nvk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute)
                .set_count(2)
                .set_name("Hair Compute")
                .enable_ring_mode();
            m_comp_descriptor = std::make_unique<nvk::Descriptor>(descriptor_create_info, device);

            nvk::PipelineCreateInfo pipeline_create_info;
            pipeline_create_info
                .add_push_constant({ vk::ShaderStageFlagBits::eCompute, 0, sizeof(ComputePushConstant) })
                .add_descriptor_set_layout(m_comp_descriptor->layout())
                .set_pipeline_type(nvk::PipelineType::eCompute)
                .add_shader("sim.comp.hlsl.spv", vk::ShaderStageFlagBits::eCompute)
                .set_name("Hair");
            m_comp_pipeline = std::make_unique<nvk::Pipeline>(pipeline_create_info, device);
        }
    }

    void HairRenderer::render(const uint32_t current_frame,
                              const float dt,
                              const HairModel& hair_model,
                              const ns::CameraData& camera_data,
                              const vk::CommandBuffer& command_buffer) const
    {
        auto hair_marker = vk::DebugUtilsLabelEXT()
            .setColor(std::array{ 0.9176f, 0.4627f, 0.796f, 1.0f })
            .setPLabelName("Hair");
        command_buffer.beginDebugUtilsLabelEXT(&hair_marker);

        m_uniform_buffers[current_frame]->set_data(&camera_data);

        vk::DescriptorBufferInfo buffer_info = { m_uniform_buffers[current_frame]->buffer(), 0, sizeof(ns::CameraData)};
        auto write_info = nvk::DescriptorWriteInfo()
            .set_set_index(current_frame)
            .add_uniform_buffer(0, buffer_info);
        m_descriptor->write(write_info);

        auto& pos_buffs = hair_model.position_buffers();
        auto& current_buffer = pos_buffs[current_frame];
        vk::DescriptorBufferInfo curr_buf = { current_buffer->buffer(), 0, current_buffer->size() };

        auto& future_buffer = pos_buffs[(current_frame == 0 ? 1 : 0)];
        vk::DescriptorBufferInfo future_buf = { future_buffer->buffer(), 0, future_buffer->size() };
        auto write_info2 = nvk::DescriptorWriteInfo()
            .set_set_index(current_frame)
            .add_storage_buffer(0, curr_buf)
            .add_storage_buffer(1, future_buf);
        m_comp_descriptor->write(write_info2);

        auto comp_marker = vk::DebugUtilsLabelEXT()
            .setColor(std::array{ 0.5803f, 0.8862f, 0.8352f, 1.0f })
            .setPLabelName("Hair Simulation");
        command_buffer.beginDebugUtilsLabelEXT(&comp_marker);

        const ComputePushConstant comp_pcs(dt, hair_model.vertex_count());

        m_comp_pipeline->bind(command_buffer);
        command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, m_comp_pipeline->layout(), 0, 1, &m_comp_descriptor->set(current_frame), 0, nullptr);
        command_buffer.pushConstants(m_comp_pipeline->layout(), vk::ShaderStageFlagBits::eCompute, 0, sizeof(ComputePushConstant), &comp_pcs);
        command_buffer.dispatch(1024, 1, 1);

        command_buffer.endDebugUtilsLabelEXT();

        auto mesh_marker = vk::DebugUtilsLabelEXT()
            .setColor(std::array{ 0.9176f, 0.4627f, 0.796f, 1.0f })
            .setPLabelName("Hair Rendering");
        command_buffer.beginDebugUtilsLabelEXT(&mesh_marker);

        nvk::RenderPass::Execute()
            .with_clear_values<2>(m_clear_values)
            .with_framebuffer(m_framebuffers[current_frame])
            .with_render_area({ {0, 0}, m_render_res })
            .with_render_pass(m_render_pass)
            .execute(command_buffer, [&](const vk::CommandBuffer& cmd) {
                m_pipeline->bind(cmd);
                cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline->layout(), 0, 1, &m_descriptor->set(current_frame), 0, nullptr);

                auto buffer_addresses = hair_model.get_hair_buffer_addresses();
                const HairConstants push_constant {
                    .model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0)),
                    .buffer_lengths = glm::ivec4(hair_model.vertex_count(), hair_model.strand_count(), 0, 0),
                    .vertex_buffer = future_buffer->address(),
                    .strand_descriptions_buffers = buffer_addresses.strand_descriptions_buffer,
                };

                cmd.pushConstants(m_pipeline->layout(), vk::ShaderStageFlagBits::eTaskEXT | vk::ShaderStageFlagBits::eMeshEXT, 0, sizeof(HairConstants), &push_constant);
                hair_model.draw(cmd);
            });

        command_buffer.endDebugUtilsLabelEXT();
        command_buffer.endDebugUtilsLabelEXT();
    }
}