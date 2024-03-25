cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)

set("3RD" "../../../ThirdParty")

# ndx12
set(ndx12 "ndx12")

add_library(${ndx12}
    IWindow.hpp Size.hpp Utility.hpp

    Context.hpp Context.cpp
    Instance.hpp Instance.cpp
    Device.hpp Device.cpp
    Swapchain.hpp Swapchain.cpp
    pipeline/Shader.hpp pipeline/Shader.cpp
    pipeline/Pipeline.hpp
    _rhi_enums.hpp
    _rhi_enum_to_string.hpp
    _rhi_struct.hpp
    _rhi.hpp
    _rhi_dx12_enums.hpp
    pipeline/Pipeline.cpp
)
target_include_directories(${ndx12} PRIVATE "${3RD}/DirectX/include/directx")
target_link_libraries(${ndx12} PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
target_compile_definitions(${ndx12} PRIVATE WIN32_LEAN_AND_MEAN _AMD64_=1)
