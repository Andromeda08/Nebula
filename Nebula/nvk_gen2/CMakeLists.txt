cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)

add_executable("nvk_gen2"
    include/nvk/VulkanRHI.hpp src/VulkanRHI.cpp
    include/nvk/VulkanExtension.hpp src/VulkanExtension.cpp
    src/test.cpp
    include/nvk/VulkanLayer.hpp
    include/nvk/VulkanDevice.hpp
    include/nvk/VulkanUtil.hpp
    src/VulkanLayer.cpp
)

target_link_libraries("nvk_gen2" PUBLIC
    ${Vulkan_LIBRARIES}
)

target_include_directories("nvk_gen2" PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    "./include/nvk"
)

target_compile_definitions ("nvk_gen2" PUBLIC
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    VULKAN_RHI_DEBUG
)