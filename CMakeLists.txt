cmake_minimum_required(VERSION 3.23)
project("Nebula")
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)

# External Dependencies
find_package(Vulkan REQUIRED)

add_subdirectory(ThirdParty/glm)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)

add_subdirectory(ThirdParty/glfw)

# Nebula
add_subdirectory(Nebula/nvk)
add_subdirectory(Nebula/nebula)

# samples
add_subdirectory(Nebula/samples)

# Tests
add_subdirectory(Nebula/test)