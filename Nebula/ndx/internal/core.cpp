#include "core.hpp"

std::string to_string(const std::wstring& wstring)
{
    std::string result;
    std::ranges::transform(wstring, std::back_inserter(result), [] (const wchar_t c) {
        return static_cast<char>(c);
    });
    return result;
}

std::string to_string(D3D12_COMMAND_LIST_TYPE type)
{
    switch (type)
    {
        case D3D12_COMMAND_LIST_TYPE_BUNDLE:        return "Bundle";
        case D3D12_COMMAND_LIST_TYPE_COMPUTE:       return "Compute";
        case D3D12_COMMAND_LIST_TYPE_COPY:          return "Copy";
        case D3D12_COMMAND_LIST_TYPE_DIRECT:        return "Direct";
        case D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE:  return "Video Decode";
        case D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE:  return "Video Encode";
        case D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS: return "Video Process";
        default:                                    throw std::invalid_argument("Unknown D3D12_COMMAND_LIST_TYPE");
    }
}

namespace Nebula::ndx
{
    std::string _ndx_pfx()
    {
        return fmt::format("{}", fmt::styled("NDX", fmt::fg(fmt::color::green_yellow)));
    }

    std::string _ndx_ok()
    {
        return fmt::format("[{} | {}]", _ndx_pfx(), fmt::styled("Info", fmt::fg(fmt::color::cornflower_blue)));
    }

    std::string _ndx_warn()
    {
        return fmt::format("[{} | {}]", _ndx_pfx(), fmt::styled("Warm", fmt::fg(fmt::color::light_golden_rod_yellow)));
    }

    std::string _ndx_fail()
    {
        return fmt::format("[{} | {}]", _ndx_pfx(), fmt::styled("Fail", fmt::fg(fmt::color::crimson)));
    }

    fmt::color to_color(D3D12_COMMAND_LIST_TYPE type)
    {
        switch (type)
        {
            case D3D12_COMMAND_LIST_TYPE_COMPUTE:   return fmt::color::aqua;
            case D3D12_COMMAND_LIST_TYPE_COPY:      return fmt::color::blanched_almond;
            case D3D12_COMMAND_LIST_TYPE_DIRECT:    return fmt::color::hot_pink;
            default:                                return fmt::color::cornflower_blue;
        }
    }
}

namespace Nebula
{
    Size2D::Size2D(uint32_t w, uint32_t h)
    : width(w), height(h)
    {
    }

    auto& Size2D::setWidth(uint32_t value)
    {
        width = value;
        return *this;
    }

    auto& Size2D::setHeight(uint32_t value)
    {
        height = value;
        return *this;
    }
}
