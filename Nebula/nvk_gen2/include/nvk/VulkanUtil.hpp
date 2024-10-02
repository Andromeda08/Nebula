#pragma once

#include <iostream>
#include <stdexcept>

#ifdef VULKAN_RHI_DEBUG
    #define VulkanRHILogDebug(MESSAGE) \
        std::cout << MESSAGE << std::endl;
#else
    #define VulkanRHILogDebug(MESSAGE)
#endif

#ifdef VULKAN_RHI_DEBUG
#define VulkanRHIThrow(MESSAGE)             \
        std::cout << MESSAGE << std::endl;  \
        throw std::runtime_error(MESSAGE);
#else
#define VulkanRHIThrow(MESSAGE)             \
    throw std::runtime_error(MESSAGE);
#endif

#define VULKAN_RHI_DISABLE_COPY(type)   \
type(const type&) = delete;             \
type& operator=(const type&) = delete;

#define VULKAN_RHI_STRUCT_PARAM(param_t, param_name, param_init)    \
param_t param_name = param_init;                                    \
inline auto& set_##param_name(const param_t& value) {               \
    param_name = value;                                             \
    return *this;                                                   \
}

#define VULKAN_RHI_STRUCT_LIST_PARAM(param_t, param_name)           \
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
