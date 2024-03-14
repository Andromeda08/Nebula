cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ndx12
set(ndx12 "ndx12")

add_library(${ndx12}
    IWindow.hpp
    Context.hpp Context.cpp
    Device.hpp Device.cpp
    Swapchain.hpp
    Utility.hpp
    Swapchain.cpp
)
target_link_libraries(${ndx12} PRIVATE "d3d12.lib" "dxgi.lib")