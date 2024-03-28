cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE Headers "./*.hpp")

add_library("nvk"
    src/Barrier.cpp
    src/Buffer.cpp
    src/Command.cpp
    src/Context.cpp
    src/Descriptor.cpp
    src/Device.cpp
    src/DeviceExtensions.cpp
    src/Image.cpp
    src/Instance.cpp
    src/Queue.cpp
    src/Swapchain.cpp
    src/Utility.cpp

    src/render/Framebuffer.cpp include/nvk/render/Framebuffer.hpp
    src/render/Pipeline.cpp
    src/render/PipelineCreateInfo.cpp
    src/render/PipelineState.cpp
    src/render/RenderPass.cpp
    src/render/Shader.cpp
)

target_link_libraries("nvk" PRIVATE ${Vulkan_LIBRARIES})
target_include_directories("nvk" PRIVATE ${Vulkan_INCLUDE_DIRS} "../nlog" "./include/nvk")
target_compile_definitions ("nvk" PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    NVK_DEBUG
    NLOG_COLORED NLOG_PRINT_EXCEPTIONS)