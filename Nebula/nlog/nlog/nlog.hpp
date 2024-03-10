#pragma once

#include <exception>
#include <format>
#include <stdexcept>
#include <string>
#include <type_traits>

#ifdef NLOG_PRINT_EXCEPTIONS
#include <iostream>
#endif

#if defined(NLOG_COLORED) && !defined(COLOR_METHOD)
#define COLOR_METHOD(COLOR, CODE)                       \
inline std::string COLOR(const std::string& str) {      \
    return CODE + str + "\x1b[0m";                      \
}
#else
#define COLOR_METHOD(COLOR, CODE)                       \
inline std::string COLOR(const std::string& str) {      \
    return str;                                         \
}
#endif

#ifndef LOGGER_FMT_METHOD
#define LOGGER_FMT_METHOD(level)                                                                                    \
template <typename... Args>                                                                                         \
inline std::string fmt_##level(std::format_string<Args...> fmt, Args&& ...args) {                                   \
    return std::format("{}{}{} {}", p_open, p_##level, p_close, std::format(fmt, std::forward<Args>(args)...));     \
}
#endif

namespace Nebula::nlog
{
    // Color code constants -------------------------------------------
    #pragma region
    constexpr const char* c_red     = "\x1b[31m";
    constexpr const char* c_green   = "\x1b[32m";
    constexpr const char* c_yellow  = "\x1b[33m";
    constexpr const char* c_blue    = "\x1b[34m";
    constexpr const char* c_magenta = "\x1b[35m";
    constexpr const char* c_cyan    = "\x1b[36m";
    constexpr const char* c_white   = "\x1b[37m";
    constexpr const char* c_reset   = "\x1b[37m";
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

    inline constexpr std::string reset()
    {
        #ifdef NLOG_COLORED
            return c_reset;
        #else
            return "";
        #endif
    }
    #pragma endregion

    // Formatter methods ----------------------------------------------
    #pragma region
    template <typename... Args>
    inline std::string fmt_prefixed(const std::string& prefix, std::format_string<Args...> fmt, Args&& ...args)
    {
        return std::format("[{}] {}", prefix, std::format(fmt, std::forward<Args>(args)...));
    }
    #pragma endregion

    // Logging constants ----------------------------------------------
    #pragma region
    #ifdef NLOG_COLORED
    constexpr const char* p_info        = "\x1b[36mI\x1b[0m";
    constexpr const char* p_success     = "\x1b[32mW\x1b[0m";
    constexpr const char* p_warning     = "\x1b[33mW\x1b[0m";
    constexpr const char* p_error       = "\x1b[31mE\x1b[0m";
    constexpr const char* p_verbose     = "\x1b[35mV\x1b[0m";

    constexpr const char* p_validation  = "\x1b[36mValidation\x1b[0m";
    #else
    constexpr const char* p_info        = "I";
    constexpr const char* p_success     = "W";
    constexpr const char* p_warning     = "W";
    constexpr const char* p_error       = "E";
    constexpr const char* p_verbose     = "V";

    constexpr const char* p_validation  = "Validation";
    #endif

    constexpr const char* p_open    = "[";
    constexpr const char* p_close   = "]";
    #pragma endregion

    // Logging format methods -----------------------------------------
    #pragma region
    LOGGER_FMT_METHOD(info)
    LOGGER_FMT_METHOD(success)
    LOGGER_FMT_METHOD(warning)
    LOGGER_FMT_METHOD(error)
    LOGGER_FMT_METHOD(verbose)
    LOGGER_FMT_METHOD(validation)
    #pragma endregion

    // Exceptions
    #pragma region
    template <typename E = std::runtime_error, typename... Args>
    inline E make_exception(std::format_string<Args...> fmt, Args&& ...args)
    {
        static_assert(std::is_base_of_v<std::exception, E>, "Template parameter E must be a type of std::exception");
        const std::string message = std::format(fmt, std::forward<Args>(args)...);
        #ifdef NLOG_PRINT_EXCEPTIONS
        std::cout << std::format("{}{}{} {}", p_open, red("Exception"), p_close, message) << std::endl;
        #endif
        return E(message);
    }
    #pragma endregion
}