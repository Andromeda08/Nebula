cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Vulkan REQUIRED)

set("NLOG" "../../nlog")

# nvk
set (Nebula_nvk "nvk")
add_library(${Nebula_nvk}
    # nvk
    # Barrier.hpp
    Buffer.hpp Buffer.cpp
    Command.hpp Command.cpp
    Context.hpp Context.cpp
    Descriptor.hpp Descriptor.cpp
    Device.hpp Device.cpp
    DeviceExtensions.hpp
    Image.hpp Image.cpp
    Instance.hpp Instance.cpp
    IWindow.hpp
    Queue.hpp Queue.cpp
    Swapchain.hpp Swapchain.cpp
    Utility.hpp

    render/Framebuffer.hpp render/Framebuffer.cpp
    render/Pipeline.hpp render/Pipeline.cpp
    render/PipelineCreateInfo.hpp render/PipelineCreateInfo.cpp
    render/PipelineState.hpp render/PipelineState.cpp
    render/PipelineType.hpp
    render/RenderPass.hpp render/RenderPass.cpp
    render/Shader.hpp render/Shader.cpp
    render/VertexInput.hpp

    # rt/BLAS.hpp
    # rt/ShaderBindingTable.hpp
    # rt/TLAS.hpp
)

target_link_libraries(${Nebula_nvk} PRIVATE ${Vulkan_LIBRARIES})
target_include_directories(${Nebula_nvk} PRIVATE ${Vulkan_INCLUDE_DIRS} ${NLOG})
target_compile_definitions (${Nebula_nvk} PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    NVK_DEBUG NVK_VERBOSE
    NLOG_COLORED NLOG_PRINT_EXCEPTIONS)