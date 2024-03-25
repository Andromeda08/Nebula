#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "_rhi_enums.hpp"

// TODO: NDX Specific Classes, change to RHI types later
namespace Nebula::ndx
{
    class Shader;
}

#define NRHI_DX12
#ifdef NRHI_DX12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12.h>
#include "_rhi_dx12_enums.hpp"

#endif

#ifdef NRHI_VK
#include <vulkan/vulkan.hpp>
#endif

#pragma region "Macros"
#ifndef struct_param
#define struct_param(struct_t, param_t, param_name, param_init) \
param_t param_name = param_init;                                \
inline struct_t& set_##param_name(const param_t& value) {       \
    param_name = value;                                         \
    return *this;                                               \
}
#endif

#ifndef DISABLE_COPY
#define DISABLE_COPY(t)         \
t(const t&) = delete;           \
t& operator=(const t&) = delete;
#endif
#pragma endregion

namespace Nebula::ndx
{
    // Generic
    #pragma region
    struct Size2D
    {
        struct_param(Size2D, uint32_t, width, 0);
        struct_param(Size2D, uint32_t, height, 0);
    };

    struct Size3D
    {
        struct_param(Size3D, uint32_t, width, 0);
        struct_param(Size3D, uint32_t, height, 0);
        struct_param(Size3D, uint32_t, depth, 0);
    };

    struct Offset2D
    {
        struct_param(Offset2D, uint32_t, x, 0);
        struct_param(Offset2D, uint32_t, y, 0);
    };

    struct Rect2D
    {
        struct_param(Rect2D, Offset2D, offset, Offset2D(0, 0));
        struct_param(Rect2D, Size2D,   extent, Size2D(0, 0));

    #ifdef NRHI_DX12
        inline explicit operator CD3DX12_RECT() const
        {
            return CD3DX12_RECT(
                static_cast<LONG>(offset.x), static_cast<LONG>(offset.y),
                static_cast<LONG>(extent.width), static_cast<LONG>(extent.height)
            );
        }
    #endif
    };

    struct Viewport
    {
        struct_param(Viewport, float, x,         0.0f);
        struct_param(Viewport, float, y,         0.0f);
        struct_param(Viewport, float, width,     0.0f);
        struct_param(Viewport, float, height,    0.0f);
        struct_param(Viewport, float, min_depth, 0.0f);
        struct_param(Viewport, float, max_depth, 1.0f);

    #ifdef NRHI_DX12
        inline explicit operator CD3DX12_VIEWPORT() const
        {
            return CD3DX12_VIEWPORT(x, y, width, height, min_depth, max_depth);
        }
    #endif
    };
    #pragma endregion

    // Shader Creation ------------------------------------------------
    struct ShaderCreateInfo
    {
        struct_param(ShaderCreateInfo, ShaderStage, shader_stage, ShaderStage::eVertex);
        struct_param(ShaderCreateInfo, std::string, file_path, {});
        struct_param(ShaderCreateInfo, std::string, entry_point, "main");
    };

    // Pipeline Vertex Input Layout -----------------------------------
    struct VertexAttributeDesc
    {
        // [Vulkan: location] - [D3D12: semantic_index]
        struct_param(VertexAttributeDesc, uint32_t,    location, 0);
        // [Vulkan: binding] - [D3D12: input_slot]
        struct_param(VertexAttributeDesc, uint32_t,    binding,  0);
        struct_param(VertexAttributeDesc, Format,      format,   Format::eUnknown);
        struct_param(VertexAttributeDesc, uint32_t,    offset,   0);

        // Ignored when using Vulkan
        struct_param(VertexAttributeDesc, std::string, semantic_name, {});
        // Ignored when using Vulkan
        struct_param(VertexAttributeDesc, bool,        append_aligned, true);
    };

    struct VertexBindingDesc
    {
        struct_param(VertexBindingDesc, VertexInputRate, input_rate, VertexInputRate::eVertex);

        // Ignored when using D3D12
        struct_param(VertexBindingDesc, uint32_t, stride, 0);
        // Ignored when using D3D12
        struct_param(VertexBindingDesc, uint32_t, binding, 0);
        // DX12 Specific
        struct_param(VertexBindingDesc, uint32_t, instance_step_rate, 0);
    };

    struct VertexInput
    {
        VertexBindingDesc                binding;
        std::vector<VertexAttributeDesc> attributes;

    #ifdef NRHI_DX12
        inline std::vector<D3D12_INPUT_ELEMENT_DESC> get_elements() const
        {
            std::vector<D3D12_INPUT_ELEMENT_DESC> result;

            for (auto& attrib : attributes)
            {
                D3D12_INPUT_ELEMENT_DESC desc = {
                    .SemanticName = attrib.semantic_name.c_str(),
                    .SemanticIndex = attrib.location,
                    .Format = convert(attrib.format),
                    .InputSlot = attrib.binding,
                    .AlignedByteOffset = (attrib.append_aligned) ? D3D12_APPEND_ALIGNED_ELEMENT : attrib.offset,
                    .InputSlotClass = convert(binding.input_rate),
                    .InstanceDataStepRate = binding.instance_step_rate,
                };
                result.push_back(desc);
            }

            return result;
        }
    #endif
    };

    // Pipeline Creation ----------------------------------------------
    struct PipelineCreateInfo
    {
        // Meta
        struct_param(PipelineCreateInfo, PipelineType, type,        PipelineType::eUnknown);
        struct_param(PipelineCreateInfo, std::string,  name,        "Unknown Pipeline");
        // Graphics
        struct_param(PipelineCreateInfo, CullMode,    cull_mode,    CullMode::eBack);
        struct_param(PipelineCreateInfo, FillMode,    fill_mode,    FillMode::eSolid);
        struct_param(PipelineCreateInfo, SampleCount, sample_count, SampleCount::e1);
        struct_param(PipelineCreateInfo, uint32_t,    target_count, 0);

        inline PipelineCreateInfo& add_shader(const std::shared_ptr<Shader>& shader)
        {
            m_shaders.push_back(shader);
            return *this;
        }

        template <typename T>
        inline PipelineCreateInfo& add_vertex_input(uint32_t base_location = 0, uint32_t binding = 0)
        {
            VertexInput input = T::vertex_input_desc(base_location, binding);
            m_vertex_inputs.push_back(input);
        }

        std::vector<std::shared_ptr<Shader>> m_shaders;
        std::vector<VertexInput>             m_vertex_inputs;
    };
}