#pragma once

#include <memory>
#include <vector>
#include <nscene/Scene.hpp>
#include <nvk/Barrier.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Command.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Device.hpp>
#include <nvk/Image.hpp>
#include <nvk/Swapchain.hpp>
#include <nvk/render/Pipeline.hpp>

namespace Nebula::nrender
{
    class Raytracer
    {
    public:
        Raytracer(const std::shared_ptr<nvk::Device>& device,
                  const std::shared_ptr<nvk::CommandPool>& command_pool,
                  const std::shared_ptr<nvk::Swapchain>& swapchain,
                  const std::shared_ptr<ns::Scene>& scene)
        : m_device(device), m_swapchain(swapchain), m_scene(scene)
        {
            using enum nvk::DescriptorType;
            using enum vk::ShaderStageFlagBits;

            auto target_create_info = nvk::ImageCreateInfo()
                .set_aspect_flags(vk::ImageAspectFlagBits::eColor)
                .set_extent(m_swapchain->extent())
                .set_format(vk::Format::eR32G32B32A32Sfloat)
                .set_name("Raytracer Target")
                .set_sample_count(vk::SampleCountFlagBits::e1)
                .set_tiling(vk::ImageTiling::eOptimal)
                .set_usage_flags(vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc)
                .set_with_sampler(true);
            m_target = nvk::Image::create(target_create_info, m_device);
            command_pool->exec_single_time_command([&](const vk::CommandBuffer& command_buffer){
                nvk::ImageBarrier(m_target, m_target->state().layout, vk::ImageLayout::eGeneral).apply(command_buffer);
            });

            auto descriptor_create_info = nvk::DescriptorCreateInfo()
                .add(eAccelerationStructure, 0, eRaygenKHR | eClosestHitKHR)
                .add(eStorageImage, 1, eRaygenKHR)
                .add(eUniformBuffer, 2, eRaygenKHR | eClosestHitKHR)
                .add(eStorageBuffer, 3, eClosestHitKHR | eCallableKHR)
                .set_count(m_swapchain->image_count())
                .set_name("Raytracer");
            m_descriptor = nvk::Descriptor::create(descriptor_create_info, m_device);

            auto pipeline_create_info = nvk::PipelineCreateInfo()
                .set_pipeline_type(nvk::PipelineType::eRayTracing)
                .add_descriptor_set_layout(m_descriptor->layout())
                .add_shader("raytracer.rgen.spv", eRaygenKHR)
                .add_shader("raytracer.rmiss.spv", eMissKHR)
                .add_shader("raytracer.rchit.spv", eClosestHitKHR)
                .set_name("Raytracer")
                .set_ray_recursion_depth(1);
            m_pipeline = nvk::Pipeline::create(pipeline_create_info, m_device);

            m_uniform_buffer.resize(m_swapchain->image_count());
            for (int32_t i = 0; i < m_uniform_buffer.size(); i++)
            {
                auto ub_create_info = nvk::BufferCreateInfo()
                    .set_buffer_type(nvk::BufferType::eUniform)
                    .set_name(std::format("Camera #{}", i))
                    .set_size(sizeof(ns::CameraData));
                m_uniform_buffer[i] = nvk::Buffer::create(ub_create_info, m_device);
            }

            auto objdbuf = m_scene->object_descriptions_buffer();
            for (int32_t i = 0; i < m_descriptor->set_count(); i++)
            {
                auto as_info = vk::WriteDescriptorSetAccelerationStructureKHR()
                    .setAccelerationStructureCount(1)
                    .setPAccelerationStructures(&m_scene->tlas()->handle());

                vk::DescriptorImageInfo  target_info = { m_target->default_sampler(), m_target->image_view(), m_target->state().layout };
                vk::DescriptorBufferInfo buffer_info = { m_uniform_buffer[i]->buffer(), 0, sizeof(ns::CameraData)};
                vk::DescriptorBufferInfo object_info = { objdbuf->buffer(), 0, objdbuf->size() };

                auto write_info = nvk::DescriptorWriteInfo()
                    .set_set_index(i)
                    .add_acceleration_structure(0, as_info)
                    .add_storage_image(1, target_info)
                    .add_uniform_buffer(2, buffer_info)
                    .add_storage_buffer(3, object_info);
                m_descriptor->write(write_info);
            }

            m_size = m_swapchain->extent();
        }

        void render(uint32_t current_frame, const vk::CommandBuffer& command_buffer) const
        {
            auto uniform_data = m_scene->active_camera()->uniform_data();
            m_uniform_buffer[current_frame]->set_data(&uniform_data);

            m_pipeline->bind(command_buffer);
            m_pipeline->bind_descriptor_set(command_buffer, m_descriptor->set(current_frame));
            m_pipeline->trace_rays(command_buffer, m_size.width, m_size.height);
        }

    private:
        vk::Extent2D                              m_size;
        std::shared_ptr<ns::Scene>                m_scene;
        std::shared_ptr<nvk::Image>               m_target;
        std::shared_ptr<nvk::Pipeline>            m_pipeline;
        std::shared_ptr<nvk::Descriptor>          m_descriptor;
        std::vector<std::shared_ptr<nvk::Buffer>> m_uniform_buffer;
        std::shared_ptr<nvk::Device>              m_device;
        std::shared_ptr<nvk::Swapchain>           m_swapchain;
    };
}