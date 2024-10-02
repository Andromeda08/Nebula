#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#define DISABLE_COPY(type)             \
type(const type&) = delete;            \
type& operator=(const type&) = delete; \

#define DEF_PRIMARY_CTOR(TYPE, ...)                         \
TYPE(__VA_ARGS__);                                          \
static std::shared_ptr<TYPE> create##TYPE(__VA_ARGS__);

#ifdef _DEBUG
    #define BEGIN_DEUBG_LABEL(NAME, LABEL, R, G, B)                  \
        const auto NAME##_marker = vk::DebugUtilsLabelEXT()          \
            .setColor(std::array{ R, G, B, 1.0f })                   \
            .setPLabelName(LABEL);                                   \
        command_buffer.beginDebugUtilsLabelEXT(&NAME##_marker);

    #define END_DEBUG_LABEL command_buffer.endDebugUtilsLabelEXT();

#else
    #define BEGIN_DEUBG_LABEL(NAME, LABEL, R, G, B)
    #define END_DEBUG_LABEL
#endif

/* Define a basic struct parameter with a chainable setter method
 * Only for use in Eikon.hpp, undef at end of file
 */
#define STRUCT_PARAM(type, name) \
type name;                       \
auto& set_##name(type value) {   \
    name = value;                \
    return *this;                \
}

namespace Eikon
{
    struct Size2D
    {
        STRUCT_PARAM(uint32_t, width);
        STRUCT_PARAM(uint32_t, height);
    };

    template <class T>
    class RingWrapper
    {
    public:
        using ValueType = T;
        using RefType   = T&;

        explicit RingWrapper(std::vector<ValueType>& vec) noexcept
        : m_size(vec.size()), m_vector(vec)
        {
        }

        explicit RingWrapper(const std::vector<ValueType>& vec) noexcept
        : m_size(vec.size()), m_vector(vec)
        {
        }

        // Get the next item in the ring
        RefType next() noexcept
        {
            m_last_item = (m_last_item + 1) % m_size;
            return m_vector[m_last_item];
        }

        // Get the last item returned from the ring
        RefType last() noexcept
        {
            return m_vector[m_last_item];
        }

        // Wraps around
        RefType operator[](const size_t index) noexcept
        {
            return m_vector[index % m_size];
        }

        // Wraps around
        void set(const size_t index, const RefType value) noexcept
        {
            m_vector[index % m_size] = value;
        }

        // Size of the ring
        [[nodiscard]] size_t size() const noexcept { return m_size; }

        [[nodiscard]] size_t last_index() const noexcept { return m_last_item; }

        // Iterator
        auto begin() const { return std::begin(m_vector); }
        auto end() const { return std::end(m_vector); }

        // Constant iterator
        auto cbegin() const { return std::cbegin(m_vector); }
        auto cend() const  { return std::cend(m_vector); }

    private:
        const size_t          m_size;
        size_t                m_last_item {0};
        const std::vector<T>& m_vector;
    };
}

#undef STRUCT_PARAM