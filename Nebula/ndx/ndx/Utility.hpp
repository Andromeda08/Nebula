#pragma once

#include <algorithm>
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

    constexpr const char* s_ndx = "ndx";

    auto const p_info = std::format("[{} | {}]", green(s_ndx), cyan("I"));
    auto const p_error = std::format("[{} | {}]", green(s_ndx), red("E"));
}