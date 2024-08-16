#pragma once

/**
 * DirectX 12, Windows and related headers
 */
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
using Microsoft::WRL::WeakRef;

/**
 * Common standard library headers
 */
#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * String and converters
 */
#define NDX_TO_WSTR(STRING) std::wstring(std::begin(STRING), std::end(STRING))

#define NDX_TO_LPCWSTR(STRING) NDX_TO_WSTR(STRING).c_str()

std::string to_string(const std::wstring& wstring);

std::string to_string(D3D12_COMMAND_LIST_TYPE type);

/**
 * NDX Pointer aliases
 */
namespace Nebula::ndx
{
    using WindowPtr          = std::shared_ptr<class IWindow>;

    // TODO: Queue abstraction
    using DX12Queue          = ComPtr<ID3D12CommandQueue>;

    using DX12ContextPtr     = std::shared_ptr<class DX12Context>;
    using DX12DevicePtr      = std::shared_ptr<class DX12Device>;
    using DX12SwapchainPtr   = std::shared_ptr<class DX12Swapchain>;
    using DX12CommandListPtr = std::shared_ptr<class DX12CommandList>;

    using DX12ShaderPtr      = std::shared_ptr<class DX12Shader>;
    using DX12PipelinePtr    = std::shared_ptr<class DX12Pipeline>;
}

/**
 * Logging utilities
 */
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/printf.h>

namespace Nebula::ndx
{
    // "NDX" prefix for logging
    std::string _ndx_pfx();

    // [NDX | Info] prefix for logging
    std::string _ndx_ok();

    // [NDX | Warn] prefix for logging
    std::string _ndx_warn();

    // [NDX | Fail] prefix for logging
    std::string _ndx_fail();

    fmt::color to_color(D3D12_COMMAND_LIST_TYPE type);
}

// NDX Info/Ok message format
#define NDX_OK(message)     "{} {}", _ndx_ok(), message

// NDX Warning message format
#define NDX_WARN(message)   "{} {}", _ndx_warn(), message

// NDX Fail/Error message format
#define NDX_FAIL(message)   "{} {}", _ndx_fail(), message

/**
 * Utility Macros
 */
#define NDX_CHECK(FN, ERROR_MSG)                                    \
if (auto result = FN; FAILED(result)) {                             \
    throw std::runtime_error(fmt::format(NDX_FAIL(ERROR_MSG)));     \
}

#define NDX_THROW(ERROR_MSG) throw std::runtime_error(fmt::format(NDX_FAIL(ERROR_MSG)))

#define NDX_DISABLE_COPY(TYPE)          \
TYPE(const TYPE&) = delete;             \
TYPE& operator=(const TYPE&) = delete;

#define NDX_DEF_PRIMARY_CTOR(TYPE, ...)                     \
TYPE(__VA_ARGS__);                                          \
static std::shared_ptr<TYPE> create##TYPE(__VA_ARGS__);

#define NDX_ENUM_TO_STRING_CASE(Enum) case Enum: return #Enum;

/**
 * Debug specific macros
 */
#ifdef _DEBUG
    #define NDX_DEBUG
    #define NDX_IF_DEBUG(X) X
#else
    #define NDX_LOG_DEBUG(X)
#endif

namespace Nebula
{
    struct Size2D
    {
        uint32_t width {0};
        uint32_t height {0};

        Size2D() = default;

        Size2D(uint32_t w, uint32_t h);

        auto& setWidth(uint32_t value);

        auto& setHeight(uint32_t value);
    };
}

/**
 * NDX struct definitions
 */
namespace Nebula::ndx
{
    enum class PipelineType
    {
        Compute,
        Graphics,
        MeshShading,
        Raytracing
    };

    inline std::string toString(const PipelineType pipelineType)
    {
        switch (pipelineType)
        {
            using enum PipelineType;
            NDX_ENUM_TO_STRING_CASE(Compute);
            NDX_ENUM_TO_STRING_CASE(Graphics);
            NDX_ENUM_TO_STRING_CASE(MeshShading);
            NDX_ENUM_TO_STRING_CASE(Raytracing);
            default: throw std::invalid_argument("Invalid PipelineType");
        }
    }

    inline fmt::color toColor(const PipelineType pipelineType)
    {
        switch (pipelineType)
        {
            case PipelineType::Compute:     return fmt::color::aqua;
            case PipelineType::Graphics:    return fmt::color::hot_pink;
            case PipelineType::MeshShading: return fmt::color::hot_pink;
            case PipelineType::Raytracing:  return fmt::color::hot_pink;
            default:                         throw std::invalid_argument("Invalid PipelineType");
        }
    }

    enum class ShaderStage
    {
        Vertex,
        TesselationControl,
        TesselationEval,
        Geometry,
        Fragment,
        Compute,
        RayGen,
        ClosestHit,
        Miss,
        Invokable,
        Mesh,
        Task
    };

    inline std::string toString(const ShaderStage shaderStage)
    {
        switch (shaderStage)
        {
            using enum ShaderStage;
            NDX_ENUM_TO_STRING_CASE(Vertex);
            NDX_ENUM_TO_STRING_CASE(TesselationControl);
            NDX_ENUM_TO_STRING_CASE(TesselationEval);
            NDX_ENUM_TO_STRING_CASE(Geometry);
            NDX_ENUM_TO_STRING_CASE(Fragment);
            NDX_ENUM_TO_STRING_CASE(Compute);
            NDX_ENUM_TO_STRING_CASE(RayGen);
            NDX_ENUM_TO_STRING_CASE(ClosestHit);
            NDX_ENUM_TO_STRING_CASE(Miss);
            NDX_ENUM_TO_STRING_CASE(Invokable);
            NDX_ENUM_TO_STRING_CASE(Mesh);
            NDX_ENUM_TO_STRING_CASE(Task);
            default: throw std::invalid_argument("Invalid ShaderStage");
        }
    }

    inline fmt::color toColor(const ShaderStage shaderStage)
    {
        switch (shaderStage)
        {
            using enum ShaderStage;
            case Vertex:               return fmt::color::hot_pink;
            case TesselationControl:   return fmt::color::green_yellow;
            case TesselationEval:      return fmt::color::green_yellow;
            case Geometry:             return fmt::color::cyan;
            case Fragment:             return fmt::color::hot_pink;
            case Compute:              return fmt::color::aqua;
            case RayGen:               return fmt::color::cornflower_blue;
            case ClosestHit:           return fmt::color::light_blue;
            case Miss:                 return fmt::color::mint_cream;
            case Invokable:            return fmt::color::magenta;
            case Mesh:                 return fmt::color::maroon;
            case Task:                 return fmt::color::maroon;
            default:                    throw std::invalid_argument("Invalid ShaderStage");
        }
    }

    struct DX12FactoryInfo
    {
        bool debugFeatures {false};

        auto& setDebugFeatures(const bool value)
        {
            debugFeatures = value;
            return *this;
        }
    };

    struct DX12DeviceInfo
    {
        bool requireDedicatedComputeQueue {true};

        auto& setRequireDedicatedComputeQueue(const bool value)
        {
            requireDedicatedComputeQueue = value;
            return *this;
        }
    };

    struct DX12SwapchainInfo
    {
        uint32_t                 backBufferCount {2};
        std::shared_ptr<IWindow> window;

        auto& setBackBufferCount(uint32_t value)
        {
            backBufferCount = value;
            return *this;
        }

        auto& setWindow(const std::shared_ptr<IWindow>& sPtr)
        {
            window = sPtr;
            return *this;
        }
    };

    struct DX12ContextInfo
    {
        DX12FactoryInfo     factoryInfo {};
        DX12DeviceInfo      deviceInfo {};
        DX12SwapchainInfo   swapchainInfo {};

        auto& setFactoryInfo(const DX12FactoryInfo& value)
        {
            factoryInfo = value;
            return *this;
        }

        auto& setDeviceInfo(const DX12DeviceInfo& value)
        {
            deviceInfo = value;
            return *this;
        }

        auto& setSwapchainInfo(const DX12SwapchainInfo& value)
        {
            swapchainInfo = value;
            return *this;
        }
    };

    struct DX12CommandListInfo
    {
        uint32_t                commandListCount {0};
        D3D12_COMMAND_LIST_TYPE type {D3D12_COMMAND_LIST_TYPE_DIRECT};

        auto& setCommandListCount(const uint32_t _commandListCount)
        {
            commandListCount = _commandListCount;
            return *this;
        }

        auto& setType(const D3D12_COMMAND_LIST_TYPE _type)
        {
            type = _type;
            return *this;
        }
    };

    struct DX12PipelineInfo
    {
        D3D12_CULL_MODE                         cullMode {D3D12_CULL_MODE_BACK};
        D3D12_FILL_MODE                         fillMode {D3D12_FILL_MODE_SOLID};
        std::vector<D3D12_INPUT_ELEMENT_DESC>   inputElements {};
        std::string                             name {"Unnamed Pipeline"};
        PipelineType                            pipelineType {PipelineType::Graphics};
        uint32_t                                renderTargetCount {0};
        std::vector<DXGI_FORMAT>                renderTargetViewFormats {};
        uint32_t                                sampleCount {1};
        std::vector<DX12ShaderPtr>              shaders {};

        auto& setCullMode(const D3D12_CULL_MODE _cullMode)
        {
            cullMode = _cullMode;
            return *this;
        }

        auto& setFillMode(const D3D12_FILL_MODE _fillMode)
        {
            fillMode = _fillMode;
            return *this;
        }

        template <class T>
        auto& addInputElement(uint32_t baseLocation = 0, uint32_t binding = 0)
        {
            const D3D12_INPUT_ELEMENT_DESC input = T::getInputElementDesc(baseLocation, binding);
            inputElements.push_back(input);
            return *this;
        }

        auto& setName(const std::string& _name)
        {
            name = _name;
            return *this;
        }

        auto& setPipelineType(const PipelineType _type)
        {
            pipelineType = _type;
            return *this;
        }

        auto& setRenderTargetCount(const uint32_t _count)
        {
            renderTargetCount = _count;
            return *this;
        }

        auto& addShader(const DX12ShaderPtr& shader)
        {
            shaders.push_back(shader);
            return *this;
        }
    };

    struct DX12ShaderInfo
    {
        ShaderStage shaderStage {ShaderStage::Vertex};
        std::string filePath {};

        auto& setShaderStage(const ShaderStage _shaderStage)
        {
            shaderStage = _shaderStage;
            return *this;
        }

        auto& setFilePath(const std::string& _filePath)
        {
            filePath = _filePath;
            return *this;
        }
    };
}
