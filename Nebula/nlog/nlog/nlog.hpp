#pragma once

#include <exception>
#include <iostream>
#include <fmt/format.h>
#include <regex>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

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
inline std::string fmt_##level(fmt::format_string<Args...> fmt, Args&& ...args) {                                   \
    return fmt::format("{}{}{} {}", p_open, p_##level, p_close, fmt::format(fmt, std::forward<Args>(args)...));     \
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

    // Color enum -----------------------------------------------------
    enum class Color
    {
       eRed,
       eGreen,
       eYellow,
       eBlue,
       eMagenta,
       eCyan,
       eWhite,
       eReset,
    };

    inline std::string to_code(const Color color)
    {
        using enum Color;
        switch (color)
        {
            case eRed:      return "\x1b[31m";
            case eGreen:    return "\x1b[32m";
            case eYellow:   return "\x1b[33m";
            case eBlue:     return "\x1b[34m";
            case eMagenta:  return "\x1b[35m";
            case eCyan:     return "\x1b[36m";
            case eWhite:    return "\x1b[37m";
            default:        return "\x1b[0m";
        }
    }

    // Formatter methods ----------------------------------------------
    #pragma region
    template <typename... Args>
    inline std::string fmt_prefixed(const std::string& prefix, fmt::format_string<Args...> fmt, Args&& ...args)
    {
        return fmt::format("[{}] {}", prefix, fmt::format(fmt, std::forward<Args>(args)...));
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

    // Exceptions -----------------------------------------------------
    #pragma region
    template <typename E = std::runtime_error, typename... Args>
    inline E make_exception(fmt::format_string<Args...> fmt, Args&& ...args)
    {
        static_assert(std::is_base_of_v<std::exception, E>, "Template parameter E must be a type of std::exception");
        const std::string message = fmt::format(fmt, std::forward<Args>(args)...);
        #ifdef NLOG_PRINT_EXCEPTIONS
        std::cout << fmt::format("{}{}{} {}", p_open, red("Exception"), p_close, message) << std::endl;
        #endif
        return E(message);
    }
    #pragma endregion

    // Logger Class ---------------------------------------------------
    class Logger
    {
    public:
        explicit Logger(std::string prefix, const Color prefix_color, bool print = true, bool store = true)
        : m_print(print), m_store(store), m_prefix_color(to_code(prefix_color)), m_prefix(std::move(prefix)) {}

        template <typename... Args>
        void info(fmt::format_string<Args...> fmt, Args&& ...args)
        {
            log(p_info, fmt, std::forward<Args>(args)...);
        }

        void info(const std::string& message)
        {
            log(p_info, "{}", message);
        }

        template <typename... Args>
        void warning(fmt::format_string<Args...> fmt, Args&& ...args)
        {
            log(p_warning, fmt, std::forward<Args>(args)...);
        }

        void warning(const std::string& message)
        {
            log(p_warning, "{}", message);
        }

        template <typename... Args>
        void error(fmt::format_string<Args...> fmt, Args&& ...args)
        {
            log(p_error, fmt, std::forward<Args>(args)...);
        }

        void error(const std::string& message)
        {
            log(p_error, "{}", message);
        }

        template <typename... Args>
        void verbose(fmt::format_string<Args...> fmt, Args&& ...args)
        {
            log(p_verbose, fmt, std::forward<Args>(args)...);
        }

        void verbose(const std::string& message)
        {
            log(p_verbose, "{}", message);
        }

    private:
        template <typename... Args>
        void log(const std::string& level, fmt::format_string<Args...> fmt, Args&& ...args) {
            std::string message = fmt::format("[{}{}\x1b[0m | {}] {}", m_prefix_color, m_prefix, level,
                                              fmt::format(fmt, std::forward<Args>(args)...));

            if (m_print) std::cout << message << std::endl;
            if (m_store) m_logs.push_back(message);
        }

        std::string strip_colors(const std::string& string)
        {
            return std::regex_replace(string, m_color_codes, "");
        }

        bool                     m_print;
        bool                     m_store;
        std::string              m_prefix_color;
        std::string              m_prefix;
        std::vector<std::string> m_logs;

        const std::regex         m_color_codes = std::regex(R"(\\x1b\[\d*m)");
    };
}