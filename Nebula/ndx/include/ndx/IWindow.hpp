#pragma once

#include <windef.h>
#include "core.hpp"

namespace Nebula::ndx
{
    class IWindow
    {
    public:
        virtual Size2D framebufferSize() const = 0;

        virtual HWND getWin32Handle() const = 0;

        virtual ~IWindow() = default;
    };
}