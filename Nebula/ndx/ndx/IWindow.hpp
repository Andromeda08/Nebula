#pragma once

#include <cstdint>
#include <vector>
#include <windef.h>

namespace Nebula::ndx
{
    class IWindow
    {
    public:
        struct Size
        {
            uint32_t width;
            uint32_t height;
        };

        virtual bool will_close() const = 0;
        virtual Size window_size() const = 0;
        virtual Size framebuffer_size() const = 0;
        virtual HWND get_win32_handle() const = 0;

        virtual ~IWindow() = default;
    };
}