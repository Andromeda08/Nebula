#pragma once

#include <vector>
#include <windef.h>
#include "Size.hpp"

namespace Nebula::ndx
{
    class IWindow
    {
    public:
        virtual bool will_close() const = 0;
        virtual Size window_size() const = 0;
        virtual Size framebuffer_size() const = 0;
        virtual HWND get_win32_handle() const = 0;

        virtual ~IWindow() = default;
    };
}