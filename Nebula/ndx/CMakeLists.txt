cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(ndx)

set("3RD" "../../ThirdParty")

add_executable("dx12_test" dx12_test.cpp dx12_Window.hpp)
target_link_libraries("dx12_test" PRIVATE ndx12 glfw)
target_include_directories("dx12_test" PRIVATE "${3RD}/glfw")