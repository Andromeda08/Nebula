#pragma once

#include <chrono>
#include <memory>
#include <nlog/nlog.hpp>

namespace Nebula::nrg
{
    struct CompilerResult
    {
        // Logging ------------------------------------------------------------
        std::unique_ptr<nlog::Logger> logger;
        std::vector<std::string>      internal_logs;

        // Compile Time Stats -------------------------------------------------
        std::chrono::time_point<std::chrono::utc_clock> start_timestamp;
        std::chrono::time_point<std::chrono::utc_clock> end_timestamp;
        std::chrono::milliseconds                       compile_time;

        // Result -------------------------------------------------------------
        bool                        success {false};
        std::optional<std::string>  failure_message {};
        std::shared_ptr<RenderPath> render_path;
    };
}