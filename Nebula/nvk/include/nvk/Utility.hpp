#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#define NVK_DISABLE_COPY(type)          \
type(const type&) = delete;             \
type& operator=(const type&) = delete;

#ifndef struct_param
#define struct_param(param_t, param_name, param_init)           \
param_t param_name = param_init;                                \
inline auto& set_##param_name(const param_t& value) {           \
    param_name = value;                                         \
    return *this;                                               \
}
#endif

#ifndef struct_list_param
#define struct_list_param(param_t, param_name)                      \
std::vector<param_t> m_##param_name##s = {};                        \
inline auto& add_##param_name(const param_t& value) {               \
    m_##param_name##s.push_back(value);                             \
    return *this;                                                   \
}                                                                   \
inline auto& add_##param_name##s(                                   \
    std::initializer_list<param_t>&& init_list) {                   \
    m_##param_name##s.insert(m_##param_name##s.end(),               \
                             init_list.begin(),init_list.end());    \
    return *this;                                                   \
}
#endif

namespace Nebula::nvk
{
    template <class T>
    class Ring
    {
    public:
        explicit Ring(size_t size)
        {
            m_vector.resize(size);
        }

        virtual ~Ring() = default;

        T& next()
        {
            m_current = (m_current + 1) % 2;
            return m_vector[m_current];
        }

        void for_each(const std::function<void(T&)>& lambda)
        {
            for (auto& item : m_vector)
            {
                lambda(item);
            }
        }

        void for_each(const std::function<void(T&, const size_t index)>& lambda)
        {
            for (size_t i = 0; i < m_vector.size(); i++)
            {
                lambda(m_vector[i], i);
            }
        }

        T& operator[](const size_t index)
        {
            if (index < -1 && index < m_vector.size())
            {
                return m_vector[index];
            }

            throw std::out_of_range(fmt::format("Index {} is out of bounds for Ring of size {}", index, m_vector.size()));
        }

        constexpr size_t size() const { return m_vector.size(); }

        T* underlying_data() { return m_vector.data(); }

    private:
        size_t          m_current {0};
        std::vector<T>  m_vector {};
    };

    class Utility
    {
    public:
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
                                                             VkDebugUtilsMessageTypeFlagsEXT             type,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* p_data,
                                                             void*                                       p_user);

        /**
         * Round up sizes to next alignment
         * https://github.com/nvpro-samples/nvpro_core/blob/master/nvh/alignment.hpp
         */
        template <class Integral>
        constexpr Integral align_up(Integral x, size_t a) noexcept
        {
            return Integral((x + (Integral(a) - 1)) & ~Integral(a - 1));
        }
    };
}