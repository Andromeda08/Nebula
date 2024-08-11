#include "DeferredLighting.hpp"
#include <nrg/resource/Resources.hpp>

namespace Nebula::nrg
{
    nrg_def_resource_requirements(DeferredLighting, ({
        std::make_shared<ImageRequirement>(s_position, ResourceUsage::eInput, ResourceType::eImage, vk::ImageLayout::eShaderReadOnlyOptimal,vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_normal, ResourceUsage::eInput, ResourceType::eImage, vk::ImageLayout::eShaderReadOnlyOptimal,vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_albedo, ResourceUsage::eInput, ResourceType::eImage, vk::ImageLayout::eShaderReadOnlyOptimal,vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_ao, ResourceUsage::eInput, ResourceType::eImage, vk::ImageLayout::eShaderReadOnlyOptimal,vk::Format::eR32Sfloat),
        std::make_shared<ImageRequirement>(s_shadows, ResourceUsage::eInput, ResourceType::eImage, vk::ImageLayout::eShaderReadOnlyOptimal,vk::Format::eR32Sfloat),
        std::make_shared<Requirement>(s_scene_data, ResourceUsage::eInput, ResourceType::eSceneData),
        std::make_shared<ImageRequirement>(s_output, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
    }))

    DeferredLighting::DeferredLighting(const std::shared_ptr<Configuration>& configuration, const std::shared_ptr<Context>& context)
    : Node("DeferredLighting", NodeType::eDeferredLighting)
    , m_configuration(*configuration)
    , m_context(context)
    , m_device(context->m_device)
    , m_current_frame(context->m_current_frame)
    {
    }

    void DeferredLighting::initialize()
    {
        auto render_resolution = m_context->m_render_resolution.operator vk::Extent2D();

        const auto& scene = get_resource<SceneResource>(s_scene_data).get_scene();
        const auto& position = get_resource<ImageResource>(s_position).get_image();
        const auto& normal = get_resource<ImageResource>(s_normal).get_image();
        const auto& albedo = get_resource<ImageResource>(s_albedo).get_image();
        const auto& output = get_resource<ImageResource>(s_output).get_image();

        using SSFB = vk::ShaderStageFlagBits;
        auto descriptor_create_info = nvk::DescriptorCreateInfo()
            .add(nvk::DescriptorType::eUniformBuffer, 0, SSFB::eFragment)
            .add(nvk::DescriptorType::eUniformBuffer, 1, SSFB::eFragment)
            .add(nvk::DescriptorType::eSampledImage, 2, SSFB::eFragment)
            .add(nvk::DescriptorType::eSampledImage, 3, SSFB::eFragment)
            .add(nvk::DescriptorType::eSampledImage, 4, SSFB::eFragment)
            .set_count(2)
            .set_name("DeferredLighting");
        m_descriptor = std::make_shared<nvk::Descriptor>(descriptor_create_info, m_device);

        auto render_pass_create_info = nvk::RenderPassCreateInfo()
            .add_attachment(output)
            .set_name("DeferredLighting")
            .set_render_area({{0,0}, render_resolution});
        m_render_pass = std::make_shared<nvk::RenderPass>(render_pass_create_info, m_device);

        auto framebuffer_create_info = nvk::FramebufferCreateInfo()
            .set_framebuffer_count(m_context->m_frames)
            .set_render_pass(m_render_pass->render_pass())
            .set_extent(render_resolution)
            .set_name("DeferredLighting Framebuffer")
            .add_attachment(output->image_view());
        m_framebuffers = std::make_shared<nvk::Framebuffer>(framebuffer_create_info, m_device);

        auto pipeline_create_info = nvk::PipelineCreateInfo()
            .set_pipeline_type(nvk::PipelineType::eGraphics)
            .add_push_constant({ SSFB::eFragment, 0, sizeof(PushConstant) })
            .add_descriptor_set_layout(m_descriptor->layout())
            .add_shader("fullscreen_quad.vert.spv", SSFB::eVertex)
            .add_shader("nrg_deferred_lighting.frag.spv", SSFB::eFragment)
            .set_attachment_count(1)
            .set_sample_count(vk::SampleCountFlagBits::e1)
            .set_render_pass(m_render_pass->render_pass())
            .set_name("DeferredLighting");
        m_pipeline = std::make_shared<nvk::Pipeline>(pipeline_create_info, m_device);

        auto camera_ubs = scene->camera_uniform_buffer();

        vk::DescriptorBufferInfo lights_info = { scene->lights_uniform_buffer()->buffer(), 0, scene->lights_uniform_buffer()->size() };

        vk::DescriptorImageInfo position_info = { position->default_sampler(), position->image_view(), vk::ImageLayout::eShaderReadOnlyOptimal };
        vk::DescriptorImageInfo normal_info   = { normal->default_sampler(), normal->image_view(), vk::ImageLayout::eShaderReadOnlyOptimal };
        vk::DescriptorImageInfo albedo_info   = { albedo->default_sampler(), albedo->image_view(), vk::ImageLayout::eShaderReadOnlyOptimal };

        for (uint32_t i = 0; i < m_descriptor->set_count(); i++)
        {
            vk::DescriptorBufferInfo camera_info = { camera_ubs[i % 2]->buffer(), 0, camera_ubs[i % 2]->size() };
            auto write_info = nvk::DescriptorWriteInfo()
                .add_uniform_buffer(0, camera_info)
                .add_uniform_buffer(1, lights_info)
                .add_combined_image_sampler(2, position_info)
                .add_combined_image_sampler(3, normal_info)
                .add_combined_image_sampler(4, albedo_info)
                .set_set_index(i);
            m_descriptor->write(write_info);
        }
    }

    void DeferredLighting::execute(const vk::CommandBuffer& command_buffer)
    {
        auto scene = get_resource<SceneResource>(s_scene_data).get_scene();

        m_render_pass->execute(command_buffer, m_framebuffers->get(m_current_frame), [&](const vk::CommandBuffer& cmd) {
            m_pipeline->bind(cmd);
            cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline->layout(), 0, 1, &m_descriptor->set(m_current_frame), 0, nullptr);
            auto push_constant = PushConstant(static_cast<int32_t>(scene->lights().size()));
            cmd.pushConstants(m_pipeline->layout(), vk::ShaderStageFlagBits::eFragment, 0, sizeof(PushConstant), &push_constant);
            cmd.draw(3, 1, 0, 0);
        });
    }

    void DeferredLighting::update()
    {

    }

    void DeferredLighting::Configuration::render()
    {
        ImGui::PushItemWidth(128);
        {
            ImGui::Checkbox("Use Ambient Occlusion", &use_ambient_occlusion);

            if (ImGui::BeginCombo("Shadows", m_shadow_modes[shadow_mode_idx].c_str()))
            {
                for (int n = 0; n < m_shadow_modes.size(); n++)
                {
                    const bool is_selected = (shadow_mode_idx == n);
                    if (ImGui::Selectable(m_shadow_modes[n].c_str(), is_selected))
                        shadow_mode_idx = n;

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

        }
        ImGui::PopItemWidth();
    }
}