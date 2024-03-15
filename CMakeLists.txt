cmake_minimum_required(VERSION 3.23)
project("Nebula")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_definitions(WIN32_LEAN_AND_MEAN)

# External Dependencies
find_package(Vulkan REQUIRED)

add_subdirectory(ThirdParty/glm)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_ENABLE_EXPERIMENTAL)

add_subdirectory(ThirdParty/glfw)

add_subdirectory(ThirdParty/json)

add_subdirectory(ThirdParty/stduuid)

add_subdirectory(ThirdParty/DirectX)

# Nebula
add_subdirectory(Nebula/nvk)
add_subdirectory(Nebula/ndx)
add_subdirectory(Nebula/nebula)