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

    src/rt/BLAS.cpp
    src/rt/TLAS.cpp
    src/rt/ShaderBindingTable.cpp
)

target_link_libraries("nvk" PUBLIC
    ${Vulkan_LIBRARIES}
)

target_include_directories("nvk" PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    "./include/nvk"
    "./internal"
)

target_compile_definitions ("nvk" PUBLIC
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    NVK_DEBUG
)