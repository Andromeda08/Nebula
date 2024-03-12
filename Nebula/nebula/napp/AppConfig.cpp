#include "AppConfig.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <nlohmann/json.hpp>
#include <nlog/nlog.hpp>

namespace Nebula
{
    AppConfig AppConfig::load(const std::string& path2json)
    {
        std::fstream config_file(path2json);
        if (!config_file.is_open())
        {
            std::cout << nlog::fmt_error("Failed to load app configuration file: {}, Using defaults", path2json) << std::endl;
            return {};
        }

        nlohmann::json data = nlohmann::json::parse(config_file);
        return {
            .name = data["name"].get<std::string>(),
            .gui_font = data["font"].get<std::string>(),
            .ray_tracing = data["ray_tracing"].get<bool>(),
            .debug_mode = data["debug_mode"].get<bool>(),
            .gui_enabled = data["gui_enabled"].get<bool>(),
            .wnd_width = data["wnd_width"].get<uint32_t>(),
            .wnd_height = data["wnd_height"].get<uint32_t>(),
            .wnd_fullscreen = data["wnd_fullscreen"].get<bool>()
        };
    }
}
