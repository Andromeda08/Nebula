#pragma once

#include <array>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <nhair/HairModel.hpp>
#include <nhair/HairRenderer.hpp>
#include <nmath/Utility.hpp>
#include <nscene/Camera.hpp>
#include <nscene/Scene.hpp>
#include <nscene/Vertex.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Command.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Device.hpp>
#include <nvk/Image.hpp>
#include <nvk/Swapchain.hpp>
#include <nvk/render/Framebuffer.hpp>
#include <nvk/render/Pipeline.hpp>
#include <nvk/render/RenderPass.hpp>

namespace Nebula::nrender
{
    struct Pass {
        Pass() = default;

        Pass(nvk::RenderPassCreateInfo&          render_pass_create_info,
             nvk::FramebufferCreateInfo&         framebuffer_create_info,
             nvk::PipelineCreateInfo&            pipeline_create_info,
             const std::shared_ptr<nvk::Device>& device)
        {
            render_pass  = nvk::RenderPass::create(render_pass_create_info, device);

            framebuffer_create_info.set_render_pass(render_pass->render_pass());
            framebuffers = nvk::Framebuffer::create(framebuffer_create_info, device);

            pipeline_create_info.set_render_pass(render_pass->render_pass());
            pipeline     = nvk::Pipeline::create(pipeline_create_info, device);
        }

        std::shared_ptr<nvk::Pipeline>    pipeline;
        std::shared_ptr<nvk::RenderPass>  render_pass;
        std::shared_ptr<nvk::Framebuffer> framebuffers;
    };

    class SBuffer
    {
    public:
        SBuffer(const vk::Extent2D&                      render_extent,
                const std::shared_ptr<nvk::Device>&      device,
                const std::shared_ptr<nvk::CommandPool>& command_pool,
                const std::shared_ptr<nvk::Swapchain>&   swapchain,
                const std::shared_ptr<ns::Scene>&        scene,
                const std::shared_ptr<nhair::HairModel>& hair_model)
        : m_image_count(swapchain->image_count())
        , m_render_extent(render_extent)
        , m_scene(scene)
        , m_hair_model(hair_model)
        , m_device(device)
        , m_command_pool(command_pool)
        {
            init_fragment_count_resources();
            init_fragment_count_init_pass();
            init_fragment_count_pass();
        }

        void init(const vk::CommandBuffer& command_buffer)
        {
            execute_fragment_count_init(command_buffer);
        }

        void execute(uint32_t current_frame, const vk::CommandBuffer& command_buffer)
        {
            execute_fragment_count_pass(current_frame, command_buffer);
        }

    private:
        // Meta
        uint32_t                            m_image_count {2};
        vk::Extent2D                        m_render_extent;

        using IUFB = vk::ImageUsageFlagBits;
        using SSFB = vk::ShaderStageFlagBits;

        // Fragment Count Pass
        // No depth testing, fragment count attachment blend op: AddOne
        #pragma region Fragment Count Pass

        void init_fragment_count_resources()
        {
            #pragma region ImageCreateInfo
            auto fragment_count_ici = nvk::ImageCreateInfo()
                .set_aspect_flags(vk::ImageAspectFlagBits::eColor)
                .set_extent(m_render_extent)
                .set_format(vk::Format::eR32Sfloat)
                .set_name("Fragment Count")
                .set_sample_count(vk::SampleCountFlagBits::e1)
                .set_tiling(vk::ImageTiling::eOptimal)
                .set_usage_flags(IUFB::eColorAttachment | IUFB::eSampled);
            #pragma endregion
            m_fragment_count = nvk::Image::create(fragment_count_ici, m_device);

            auto descriptor_create_info = nvk::DescriptorCreateInfo()
                .set_count(m_image_count)
                .set_name("Fragment Count")
                .add(nvk::DescriptorType::eUniformBuffer, 0, SSFB::eVertex | SSFB::eFragment | SSFB::eMeshEXT);
            m_fragment_count_descriptor = nvk::Descriptor::create(descriptor_create_info, m_device);

            m_camera_uniform.resize(m_image_count);
            for (int32_t i = 0; i < m_camera_uniform.size(); i++)
            {
                auto ub_create_info = nvk::BufferCreateInfo()
                    .set_buffer_type(nvk::BufferType::eUniform)
                    .set_name(std::format("Camera #{}", i))
                    .set_size(sizeof(ns::CameraData));
                m_camera_uniform[i] = nvk::Buffer::create(ub_create_info, m_device);

                vk::DescriptorBufferInfo buffer_info = { m_camera_uniform[i]->buffer(), 0, sizeof(ns::CameraData)};
                auto write_info = nvk::DescriptorWriteInfo()
                    .set_set_index(i)
                    .add_uniform_buffer(0, buffer_info);
                m_fragment_count_descriptor->write(write_info);
            }
        }

        void init_fragment_count_init_pass()
        {
            auto render_pass_create_info = nvk::RenderPassCreateInfo()
                .add_attachment(m_fragment_count)
                .set_name("Fragment Count Init")
                .set_render_area({{0, 0}, m_render_extent});

            auto framebuffer_create_info = nvk::FramebufferCreateInfo()
                .set_framebuffer_count(1)
                .add_attachment(m_fragment_count->image_view(), 0)
                .set_extent(m_render_extent)
                .set_name("Fragment Count Init");

            auto pipeline_create_info = nvk::PipelineCreateInfo()
                .set_pipeline_type(nvk::PipelineType::eGraphics)
                .add_shader("fullscreen_quad.vert.spv", SSFB::eVertex)
                .add_shader("sb_0_init.frag.spv", SSFB::eFragment)
                .set_sample_count(vk::SampleCountFlagBits::e1)
                .add_attachment()
                .set_name("Fragment Count Init");

            m_fragment_count_init_pass = Pass(render_pass_create_info,
                                              framebuffer_create_info,
                                              pipeline_create_info,
                                              m_device);

            m_fragment_count_init_pass_label = vk::DebugUtilsLabelEXT()
                .setPLabelName("Fragment Count Init")
                .setColor(std::array{ nmath::randf(), nmath::randf(), nmath::randf(), 1.0f });
        }

        void execute_fragment_count_init(const vk::CommandBuffer& command_buffer)
        {
            if (m_fragment_count_initialized) return;

            Pass& pass = m_fragment_count_init_pass;

            command_buffer.beginDebugUtilsLabelEXT(m_fragment_count_init_pass_label);
            pass.render_pass->execute(command_buffer, pass.framebuffers->get(0), [&](const vk::CommandBuffer& cmd){
                pass.pipeline->bind(cmd);
                cmd.draw(3, 1, 0, 0);
            });
            command_buffer.endDebugUtilsLabelEXT();

            m_fragment_count_initialized = true;
        }

        void init_fragment_count_pass()
        {
            auto render_pass_create_info = nvk::RenderPassCreateInfo()
                .add_attachment(m_fragment_count)
                .set_name("Fragment Count")
                .set_render_area({{0, 0}, m_render_extent});

            auto framebuffer_create_info = nvk::FramebufferCreateInfo()
                .set_framebuffer_count(m_image_count)
                .add_attachment(m_fragment_count->image_view(), 0)
                .set_extent(m_render_extent)
                .set_name("Fragment Count");

            using CCFB = vk::ColorComponentFlagBits;
            auto fragment_count_blend_state = vk::PipelineColorBlendAttachmentState()
                .setColorWriteMask(CCFB::eR)
                .setBlendEnable(true)
                .setAlphaBlendOp(vk::BlendOp::eAdd)
                .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
                .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
                .setColorBlendOp(vk::BlendOp::eAdd)
                .setSrcColorBlendFactor(vk::BlendFactor::eOne)
                .setDstColorBlendFactor(vk::BlendFactor::eOne);

            auto pipeline_create_info = nvk::PipelineCreateInfo()
                .set_pipeline_type(nvk::PipelineType::eGraphics)
                .add_push_constant({ SSFB::eVertex | SSFB::eFragment, 0, sizeof(ns::ObjectPushConstant)})
                .add_descriptor_set_layout(m_fragment_count_descriptor->layout())
                .add_shader("sb_1_fragment_count.vert.spv", SSFB::eVertex)
                .add_shader("sb_1_fragment_count.frag.spv", SSFB::eFragment)
                .add_attribute_descriptions<ns::Vertex>()
                .add_binding_description<ns::Vertex>()
                .set_sample_count(vk::SampleCountFlagBits::e1)
                .add_attachment(fragment_count_blend_state)
                .set_name("Fragment Count");

            m_fragment_count_pass = Pass(render_pass_create_info, framebuffer_create_info,
                                         pipeline_create_info, m_device);

            m_fragment_count_pass_label = vk::DebugUtilsLabelEXT()
                .setPLabelName("Fragment Count")
                .setColor(std::array{ nmath::randf(), nmath::randf(), nmath::randf(), 1.0f });

            auto hair_pipeline_create_info = nvk::PipelineCreateInfo()
                .add_push_constant({ SSFB::eTaskEXT | SSFB::eMeshEXT | SSFB::eFragment, 0, sizeof(nhair::HairConstants) })
                .add_descriptor_set_layout(m_fragment_count_descriptor->layout())
                .set_pipeline_type(nvk::PipelineType::eGraphics)
                .add_shader("hair.task.glsl.spv", SSFB::eTaskEXT)
                .add_shader("hair.mesh.glsl.spv", SSFB::eMeshEXT)
                .add_shader("sb_1_fragment_count.frag.spv", SSFB::eFragment)
                .add_attachment(fragment_count_blend_state)
                .set_sample_count(vk::SampleCountFlagBits::e1)
                .set_cull_mode(vk::CullModeFlagBits::eNone)
                .set_render_pass(m_fragment_count_pass.render_pass->render_pass())
                .set_name("Hair");
            m_fragment_count_hair_pipeline = std::make_shared<nvk::Pipeline>(hair_pipeline_create_info, m_device);
        }

        void execute_fragment_count_pass(uint32_t current_frame, const vk::CommandBuffer& command_buffer)
        {
            Pass& pass = m_fragment_count_pass;

            auto camera_data = m_scene->active_camera()->uniform_data();
            m_camera_uniform[current_frame]->set_data(&camera_data);

            command_buffer.beginDebugUtilsLabelEXT(m_fragment_count_pass_label);
            pass.render_pass->execute(command_buffer, pass.framebuffers->get(current_frame), [&](const vk::CommandBuffer& cmd){
                pass.pipeline->bind(cmd);
                pass.pipeline->bind_descriptor_set(cmd, m_fragment_count_descriptor->set(current_frame));
                for (const auto& object : m_scene->objects()) {
                    auto push_constant = object.get_push_constants();
                    pass.pipeline->push_constants<ns::ObjectPushConstant>(cmd, SSFB::eVertex | SSFB::eFragment, 0, &push_constant);
                    object.mesh->draw(cmd);
                }

                auto buffer_addresses = m_hair_model->get_hair_buffer_addresses();
                nhair::HairConstants hair_push_constant {
                    .model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0)),
                    .buffer_lengths = glm::ivec4(m_hair_model->vertex_count(), m_hair_model->strand_count(), 0, 0),
                    .hair_diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                    .hair_specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                    .vertex_buffer = buffer_addresses.vertex_buffer,
                    .strand_descriptions_buffers = buffer_addresses.strand_descriptions_buffer,
                };

                m_fragment_count_hair_pipeline->bind(command_buffer);
                m_fragment_count_hair_pipeline->bind_descriptor_set(cmd, m_fragment_count_descriptor->set(current_frame));
                m_fragment_count_hair_pipeline->push_constants<nhair::HairConstants>(command_buffer,
                                                                                     SSFB::eTaskEXT | SSFB::eMeshEXT | SSFB::eFragment,
                                                                                     0, &hair_push_constant);
                m_hair_model->draw(cmd);
            });
            command_buffer.endDebugUtilsLabelEXT();
        }

    public:
        std::shared_ptr<nvk::Image>         m_fragment_count;                     // R32Sfloat
    private:

        Pass                                m_fragment_count_init_pass;
        vk::DebugUtilsLabelEXT              m_fragment_count_init_pass_label;
        bool                                m_fragment_count_initialized {false};

        Pass                                m_fragment_count_pass;
        std::shared_ptr<nvk::Pipeline>      m_fragment_count_hair_pipeline;
        vk::DebugUtilsLabelEXT              m_fragment_count_pass_label;
        std::shared_ptr<nvk::Descriptor>    m_fragment_count_descriptor;

        std::shared_ptr<nhair::HairModel>   m_hair_model;
        std::shared_ptr<ns::Scene>          m_scene;
        std::vector<std::shared_ptr<nvk::Buffer>> m_camera_uniform;

        #pragma endregion

        // Memory address computation

        // Fragment store pass

        // Resolve Pass

        std::shared_ptr<nvk::Device>        m_device;
        std::shared_ptr<nvk::CommandPool>   m_command_pool;
    };
}