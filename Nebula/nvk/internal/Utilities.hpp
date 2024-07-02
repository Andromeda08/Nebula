#pragma once

#include <iostream>
#include <format>
#include <stdexcept>
#include <string>
#include <type_traits>

#pragma region "Configuration"
// Log levels
#define NVK_LOG_LEVEL_NONE    0
#define NVK_LOG_LEVEL_INFO    1
#define NVK_LOG_LEVEL_SUCCESS 2
#define NVK_LOG_LEVEL_WARNING 3
#define NVK_LOG_LEVEL_ERROR   4
#define NVK_LOG_LEVEL_VERBOSE 5

// Options
#define NVK_LOG_COLORS
#define NVK_MSG_FORMAT "[{} | {}] {}"
#define NVK_LOG_LEVEL  NVK_LOG_LEVEL_VERBOSE

#pragma endregion

#ifdef _MSC_VER
#define NVK_PRINT_EXCEPTIONS
#endif

#pragma region "NVK_DEFINE_FMT_METHOD"
#ifdef NVK_LOG_COLORS
#define NVK_DEFINE_FMT_METHOD(NAME, COLOR)                 \
inline static std::string NAME(const std::string& str) {   \
    return std::format(COLOR"{}\x1b[0m", str);             \
}
#else
#define NVK_DEFINE_FMT_METHOD(NAME, COLOR)          \
static std::string NAME(const std::string& str) {   \
    return str;                                     \
}
#endif
#pragma endregion

/**
 * NVK_DEFINE_FMT_STR_METHODS\n
 * This macro defines two methods for string formatting:\n
 * - [1] Takes only string as a parameter.\n
 * - [2] Takes a format string and arguments as parameters.
 * @param NAME Msg level name in method. (fmt_<NAME>)
 * @param COLOR Msg level prefix color
 * @param PREFIX Msg level prefix
 */
#pragma region "NVK_DEFINE_FMT_STR_METHODS"
#define NVK_DEFINE_FMT_STR_METHODS(NAME, COLOR, PREFIX)                         \
template <typename... Args>                                                     \
inline std::string fmt_##NAME(std::format_string<Args...> fmt, Args&& ...args)  \
{                                                                               \
    return std::format(                                                         \
        NVK_MSG_FORMAT,                                                         \
        Format::red("nvk"), Format::COLOR(PREFIX),                              \
        std::format(fmt, std::forward<Args>(args)...)                           \
    );                                                                          \
}                                                                               \
inline std::string fmt_##NAME(const std::string& str)                           \
{                                                                               \
    return std::format(NVK_MSG_FORMAT,                                          \
                       Format::red("nvk"), Format::COLOR(PREFIX), str);         \
                                                                                \
}
#pragma endregion

/**
 * NVK_DEFINE_FMT_PRINT_METHODS\n
 * This macro defines two methods for logging messages:\n
 * - [1] Takes only string as a parameter.\n
 * - [2] Takes a format string and arguments as parameters.\n
 * - Calls the appropriate "fmt_<NAME>" method to format the message.
 * @param NAME Log level name in method. (print_<NAME>)
 * @param COLOR Log level prefix color
 * @param LEVEL Log level
 */
#pragma region "NVK_DEFINE_FMT_PRINT_METHODS"
#define NVK_DEFINE_FMT_PRINT_METHODS(NAME, COLOR, LEVEL)                        \
template <typename... Args>                                                     \
inline void print_##NAME(std::format_string<Args...> fmt, Args&& ...args)       \
{                                                                               \
    if (NVK_LOG_LEVEL >= LEVEL)                                                 \
    {                                                                           \
        std::cout << fmt_##NAME(fmt, std::forward<Args>(args)...) << std::endl; \
    }                                                                           \
}                                                                               \
inline void print_##NAME(const std::string& str)                                \
{                                                                               \
    if (NVK_LOG_LEVEL >= LEVEL)                                                 \
    {                                                                           \
        std::cout << fmt_##NAME(str) << std::endl;                              \
    }                                                                           \
}
#pragma endregion

namespace Nebula::nvk
{
    struct Format
    {
        NVK_DEFINE_FMT_METHOD(red,     "\x1b[31m");
        NVK_DEFINE_FMT_METHOD(green,   "\x1b[32m");
        NVK_DEFINE_FMT_METHOD(yellow,  "\x1b[33m");
        NVK_DEFINE_FMT_METHOD(blue,    "\x1b[34m");
        NVK_DEFINE_FMT_METHOD(magenta, "\x1b[35m");
        NVK_DEFINE_FMT_METHOD(cyan,    "\x1b[36m");
        NVK_DEFINE_FMT_METHOD(white,   "\x1b[37m");
    };

    #pragma region "String formatter methods"

    NVK_DEFINE_FMT_STR_METHODS(info,    blue,    "Info");
    NVK_DEFINE_FMT_STR_METHODS(success, green,   "Success");
    NVK_DEFINE_FMT_STR_METHODS(warning, yellow,  "Warning");
    NVK_DEFINE_FMT_STR_METHODS(error,   red,     "Error");
    NVK_DEFINE_FMT_STR_METHODS(verbose, magenta, "Verbose");

    #pragma endregion

    #pragma region "Print methods"

    NVK_DEFINE_FMT_PRINT_METHODS(info,    blue,    NVK_LOG_LEVEL_INFO);
    NVK_DEFINE_FMT_PRINT_METHODS(success, green,   NVK_LOG_LEVEL_SUCCESS);
    NVK_DEFINE_FMT_PRINT_METHODS(warning, yellow,  NVK_LOG_LEVEL_WARNING);
    NVK_DEFINE_FMT_PRINT_METHODS(error,   red,     NVK_LOG_LEVEL_ERROR);
    NVK_DEFINE_FMT_PRINT_METHODS(verbose, magenta, NVK_LOG_LEVEL_VERBOSE);

    #pragma endregion

    #pragma region "Exceptions"

    template <typename E = std::runtime_error, typename... Args>
    inline E make_exception(std::format_string<Args...> fmt, Args&& ...args)
    {
        static_assert(std::is_base_of_v<std::exception, E>, "Template parameter E must be a type of std::exception");
        const std::string message = fmt_error(fmt, std::forward<Args>(args)...);
        #ifdef NVK_PRINT_EXCEPTIONS
        std::cout << message << std::endl;
        #endif
        return E(message);
    }

    template <typename E = std::runtime_error>
    inline E make_exception(const std::string& str)
    {
        static_assert(std::is_base_of_v<std::exception, E>, "Template parameter E must be a type of std::exception");
        const std::string message = fmt_error(str);
        #ifdef NVK_PRINT_EXCEPTIONS
        std::cout << message << std::endl;
        #endif
        return E(message);
    }

    #pragma endregion
}