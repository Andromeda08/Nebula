#pragma once

#include <cstdint>

namespace Nebula
{
    struct Size2D
    {
        uint32_t width {0};
        uint32_t height {0};

        inline Size2D& set_width(uint32_t w)
        {
            width = w;
            return *this;
        }

        inline Size2D& set_height(uint32_t h)
        {
            height = h;
            return *this;
        }

        inline float ratio() const
        {
            return static_cast<float>(width) / static_cast<float>(height);
        }
    };
}