#pragma once

#include <algorithm>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

#define COLOR_METHOD(COLOR, CODE)                       \
inline std::string COLOR(const std::string& str) {      \
    return CODE + str + "\x1b[0m";                      \
}

namespace Nebula
{
    // wstring conversion 🤢 -----------------------------------------
    static std::string to_str(const std::wstring& wstring)
    {
        std::string result;
        std::transform(wstring.begin(), wstring.end(), std::back_inserter(result), [] (wchar_t c) {
            return (char)c;
        });
        return result;
    }

    // Color code constants -------------------------------------------
    #pragma region
    constexpr const char* c_red     = "\x1b[31m";
    constexpr const char* c_green   = "\x1b[32m";
    constexpr const char* c_yellow  = "\x1b[33m";
    constexpr const char* c_blue    = "\x1b[34m";
    constexpr const char* c_magenta = "\x1b[35m";
    constexpr const char* c_cyan    = "\x1b[36m";
    constexpr const char* c_white   = "\x1b[37m";
    constexpr const char* c_reset   = "\x1b[0m";
    #pragma endregion

    // Color methods --------------------------------------------------
    #pragma region
    COLOR_METHOD(red,     c_red);
    COLOR_METHOD(green,   c_green);
    COLOR_METHOD(yellow,  c_yellow);
    COLOR_METHOD(blue,    c_blue);
    COLOR_METHOD(magenta, c_magenta);
    COLOR_METHOD(cyan,    c_cyan);
    COLOR_METHOD(white,   c_white);
    #pragma endregion

    // Prefix definitions --------------------------------------------
    #pragma region
    auto const s_ndx = "ndx";

    auto const p_info = std::format("[{} | {}]", green(s_ndx), cyan("I"));
    auto const p_warn = std::format("[{} | {}]", green(s_ndx), yellow("W"));
    auto const p_error = std::format("[{} | {}]", green(s_ndx), red("E"));
    #pragma endregion

    // Print methods --------------------------------------------------
    #pragma region
    template <typename... Args>
    inline void pInfo(std::format_string<Args...> fmt, Args&& ...args)
    {
        std::cout << std::format("{} {}", p_info, std::format(fmt, std::forward<Args>(args)...)) << std::endl;
    }

    template <typename... Args>
    inline void pWarn(std::format_string<Args...> fmt, Args&& ...args)
    {
        std::cout << std::format("{} {}", p_warn, std::format(fmt, std::forward<Args>(args)...)) << std::endl;
    }

    template <typename... Args>
    inline void pError(std::format_string<Args...> fmt, Args&& ...args)
    {
        std::cout << std::format("{} {}", p_error, std::format(fmt, std::forward<Args>(args)...)) << std::endl;
    }
    #pragma endregion

    // Exception utility methods --------------------------------------
    template <typename E = std::runtime_error, typename... Args>
    inline E make_exception(std::format_string<Args...> fmt, Args&& ...args)
    {
        static_assert(std::is_base_of_v<std::exception, E>, "Template parameter E must be a type of std::exception");
        const std::string message = std::format(fmt, std::forward<Args>(args)...);
        std::cout << std::format("{} {}", p_error, message) << std::endl;
        return E(message);
    }
}