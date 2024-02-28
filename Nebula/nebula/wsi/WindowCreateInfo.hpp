#pragma once

#include <cstdint>
#include <string>
#include <common/Size2D.hpp>

namespace Nebula::wsi
{
    struct WindowCreateInfo
    {
        Size2D      dimensions {1280, 720};
        bool        fullscreen {false};
        std::string title {"Nebula"};

        inline WindowCreateInfo& set_size(const Size2D& size)
        {
            dimensions = size;
            return *this;
        }

        inline WindowCreateInfo& set_width(uint32_t w)
        {
            dimensions.width = w;
            return *this;
        }

        inline WindowCreateInfo& set_height(uint32_t h)
        {
            dimensions.height = h;
            return *this;
        }

        inline WindowCreateInfo& set_fullscreen(bool f)
        {
            fullscreen = f;
            return *this;
        }

        inline WindowCreateInfo& set_title(const std::string& t)
        {
            title = t;
            return *this;
        }
    };
}