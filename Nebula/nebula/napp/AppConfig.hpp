#pragma once

#include <cstdint>
#include <string>

namespace Nebula
{
    struct AppConfig
    {
        const std::string   name            = "Nebula Application";
        const std::string   gui_font        = "JetBrainsMono-Regular.ttf";
        const bool          ray_tracing     = true;
        const bool          debug_mode      = true;
        const bool          gui_enabled     = true;
        const uint32_t      wnd_width       = 1600;
        const uint32_t      wnd_height      = 900;
        const bool          wnd_fullscreen  = false;

        static AppConfig load(const std::string& path2json)
        {
            return {};
        }
    };
}