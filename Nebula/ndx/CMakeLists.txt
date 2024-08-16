set(CMAKE_CXX_STANDARD 20)

set(DirectX12_LIBRARIES "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
set(DirectX12_INCLUDE_DIRS "../../ThirdParty/DirectX/include/directx")

file(GLOB_RECURSE Headers "./**/*.hpp")

add_library("ndx"
    internal/core.cpp
    src/DX12Context.cpp
    src/DX12Device.cpp
    src/DX12Swapchain.cpp
    src/DX12CommandList.cpp
    src/DX12Pipeline.cpp
    src/DX12Shader.cpp
)

target_link_libraries("ndx" PUBLIC
    ${DirectX12_LIBRARIES}
    fmt::fmt
)

target_compile_definitions("ndx" PUBLIC
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _AMD64_=1
)

target_include_directories("ndx" PUBLIC
    ${DirectX12_INCLUDE_DIRS}
    "./include/ndx"
    "./internal"
    "../../ThirdParty/fmt/include"
)

add_executable("ndx_test"
    test/main.cpp
)
target_link_libraries("ndx_test" PUBLIC ndx glfw)
target_include_directories("ndx" PUBLIC
    "./include/ndx"
    "./internal"
    "../../ThirdParty/fmt/include"
    "../../ThirdParty/glfw/include"
)