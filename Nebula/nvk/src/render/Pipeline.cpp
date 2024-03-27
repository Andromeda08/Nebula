#include "render/Pipeline.hpp"
#include <format>
#include <nlog/nlog.hpp>

#if defined(NVK_DEBUG) | defined(NVK_VERBOSE)
#include <iostream>
#endif

namespace Nebula::nvk
{
    Pipeline::Pipeline(PipelineCreateInfo& create_info, const std::shared_ptr<Device>& device)
    : m_device(device), m_name(create_info.m_name), m_type(create_info.m_type), m_bind_point(to_bind_point(create_info.m_type))
    {
        create_info.m_pipeline_state.update();

        if (create_info.m_shader_sources.size() < 1)
        {
            throw nlog::make_exception("Failed to create Pipeline {}: no shaders were specified", m_name);
        }

        create_pipeline_layout(create_info);

        switch (create_info.m_type)
        {
            case PipelineType::eCompute: {
                create_compute(create_info);
                break;
            }
            case PipelineType::eGraphics: {
                create_graphics(create_info);
                break;
            }
            case PipelineType::eRayTracing: {
                create_ray_tracing(create_info);
                break;
            }
            default:
                throw nlog::make_exception("Failed to create Pipeline {}: Pipeline type not specified", m_name);
        }

        m_device->name_object(std::format("{}: {} Pipeline", m_name, to_string(m_type)),
                              (uint64_t) m_pipeline.operator VkPipeline(),
                              vk::ObjectType::ePipeline);

        m_device->name_object(std::format("{}: PipelineLayout", m_name),
                              (uint64_t) m_pipeline_layout.operator VkPipelineLayout(),
                              vk::ObjectType::ePipelineLayout);

        #ifdef NVK_VERBOSE
        std::cout << nlog::fmt_info("Created {} Pipeline: {}", to_string(m_type), m_name) << std::endl;
        #endif
    }

    void Pipeline::bind(const vk::CommandBuffer& command_buffer)
    {
        command_buffer.bindPipeline(m_bind_point, m_pipeline);
    }

    Pipeline::~Pipeline()
    {
        m_device->handle().destroy(m_pipeline);
        m_device->handle().destroy(m_pipeline_layout);

        #ifdef NVK_VERBOSE
        std::cout << nlog::fmt_info("Destroyed {} Pipeline: {}", to_string(m_type), m_name) << std::endl;
        #endif
    }

    void Pipeline::create_pipeline_layout(const PipelineCreateInfo& create_info)
    {
        auto layout_info = vk::PipelineLayoutCreateInfo()
            .setSetLayoutCount(create_info.m_descriptor_set_layouts.size())
            .setPSetLayouts(create_info.m_descriptor_set_layouts.data())
            .setPushConstantRangeCount(create_info.m_push_constants.size())
            .setPPushConstantRanges(create_info.m_push_constants.data());

        if (const vk::Result result = m_device->handle().createPipelineLayout(&layout_info, nullptr, &m_pipeline_layout);
            result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to create PipelineLayout: {} ({})", m_name, to_string(result));
        }
    }

    void Pipeline::create_compute(const PipelineCreateInfo& create_info)
    {
        #ifdef NVK_DEBUG
        if (create_info.m_shader_sources.size() > 1)
        {
            std::cout << nlog::fmt_warning("[Warning] More than 1 shader specified for a Compute Pipeline, using the first specified compute shader") << std::endl;
        }
        #endif

        auto shader_info = std::ranges::find_if(create_info.m_shader_sources, [&](const ShaderCreateInfo& sci){
           return sci.shader_stage == vk::ShaderStageFlagBits::eCompute;
        });

        if (shader_info == std::end(create_info.m_shader_sources))
        {
            throw nlog::make_exception("Failed to create Compute Pipeline {}: No compute shader was specified", m_name);
        }

        auto shader = std::make_shared<Shader>(*shader_info, m_device);
        auto compute_info = vk::ComputePipelineCreateInfo()
            .setLayout(m_pipeline_layout)
            .setStage(shader->make_stage_info());

        if (const vk::Result result = m_device->handle().createComputePipelines({}, 1, &compute_info, nullptr, &m_pipeline);
            result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to create Compute Pipeline {}: {}", m_name, to_string(result));
        }
    }

    void Pipeline::create_graphics(const PipelineCreateInfo& create_info)
    {
        std::vector<std::shared_ptr<Shader>> shaders;
        std::vector<vk::PipelineShaderStageCreateInfo> shader_stage_infos;
        for (const auto& shader_info : create_info.m_shader_sources)
        {
            shaders.push_back(std::make_shared<Shader>(shader_info, m_device));
            shader_stage_infos.push_back(shaders.back()->make_stage_info());
        }

        auto graphics_info = vk::GraphicsPipelineCreateInfo()
            .setPInputAssemblyState(&create_info.m_pipeline_state.input_assembly_state)
            .setPRasterizationState(&create_info.m_pipeline_state.rasterization_state)
            .setPMultisampleState(&create_info.m_pipeline_state.multisample_state)
            .setPDepthStencilState(&create_info.m_pipeline_state.depth_stencil_state)
            .setPViewportState(&create_info.m_pipeline_state.viewport_state)
            .setPDynamicState(&create_info.m_pipeline_state.dynamic_state)
            .setPColorBlendState(&create_info.m_pipeline_state.color_blend_state)
            .setPVertexInputState(&create_info.m_pipeline_state.vertex_input_state)
            .setStageCount(shader_stage_infos.size())
            .setPStages(shader_stage_infos.data())
            .setLayout(m_pipeline_layout)
            .setPNext(nullptr);

        if (create_info.m_with_render_pass)
        {
            graphics_info.setRenderPass(create_info.m_render_pass);
        }

        if (const vk::Result result = m_device->handle().createGraphicsPipelines({}, 1, &graphics_info, nullptr, &m_pipeline);
            result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to create Graphics Pipeline: {} ({})", m_name, to_string(result));
        }
    }

    void Pipeline::create_ray_tracing(const PipelineCreateInfo& create_info)
    {
        std::vector<std::shared_ptr<Shader>> shaders;
        std::vector<vk::PipelineShaderStageCreateInfo> shader_stage_infos;
        for (const auto& shader_info : create_info.m_shader_sources)
        {
            shaders.push_back(std::make_shared<Shader>(shader_info, m_device));
            shader_stage_infos.push_back(shaders.back()->make_stage_info());
        }

        auto sh_groups = create_rt_shader_groups(shader_stage_infos);
        auto rt_info = vk::RayTracingPipelineCreateInfoKHR()
            .setFlags(vk::PipelineCreateFlagBits::eRayTracingNoNullClosestHitShadersKHR | vk::PipelineCreateFlagBits::eRayTracingNoNullMissShadersKHR)
            .setStageCount(shader_stage_infos.size())
            .setPStages(shader_stage_infos.data())
            .setGroupCount(sh_groups.size())
            .setPGroups(sh_groups.data())
            .setMaxPipelineRayRecursionDepth(create_info.m_ray_depth)
            .setLayout(m_pipeline_layout);

        if (const vk::Result result = m_device->handle().createRayTracingPipelinesKHR(nullptr, nullptr, 1, &rt_info, nullptr, &m_pipeline);
            result != vk::Result::eSuccess)
        {
            throw nlog::make_exception("Failed to create Ray tracing Pipeline: {} ({})", m_name, to_string(result));
        }
    }

    std::vector<vk::RayTracingShaderGroupCreateInfoKHR>
    Pipeline::create_rt_shader_groups(const std::vector<vk::PipelineShaderStageCreateInfo>& shader_stages)
    {
        std::vector<vk::RayTracingShaderGroupCreateInfoKHR> result;
        for (auto i = 0; i < shader_stages.size(); i++)
        {
            switch (shader_stages[i].stage)
            {
                case vk::ShaderStageFlagBits::eRaygenKHR:
                case vk::ShaderStageFlagBits::eMissKHR: {
                    auto group = vk::RayTracingShaderGroupCreateInfoKHR()
                        .setType(vk::RayTracingShaderGroupTypeKHR::eGeneral)
                        .setGeneralShader(i)
                        .setClosestHitShader(VK_SHADER_UNUSED_KHR)
                        .setAnyHitShader(VK_SHADER_UNUSED_KHR)
                        .setIntersectionShader(VK_SHADER_UNUSED_KHR);
                    result.push_back(group);
                    break;
                }
                case vk::ShaderStageFlagBits::eClosestHitKHR: {
                    auto group = vk::RayTracingShaderGroupCreateInfoKHR()
                        .setType(vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup)
                        .setGeneralShader(VK_SHADER_UNUSED_KHR)
                        .setClosestHitShader(i)
                        .setAnyHitShader(VK_SHADER_UNUSED_KHR)
                        .setIntersectionShader(VK_SHADER_UNUSED_KHR);
                    result.push_back(group);
                    break;
                }
                default: {
                    #ifdef NVK_DEBUG
                    std::cout << nlog::fmt_warning("Non-RayTracing shader ({}) specified for Pipeline {}",
                                                   to_string(shader_stages[i].stage), m_name) << std::endl;
                    #endif
                }
            }
        }
        return result;
    }
}