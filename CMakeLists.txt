cmake_minimum_required(VERSION 3.23)
project("Nebula")

# External Dependencies
find_package(Vulkan REQUIRED)

add_subdirectory(ThirdParty/glm)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_ENABLE_EXPERIMENTAL)

add_subdirectory(ThirdParty/glfw)

add_subdirectory(ThirdParty/json)

add_subdirectory(ThirdParty/stduuid)

set(TINYGLTF_USE_CPP14 ON)
set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
add_subdirectory(ThirdParty/tinygltf)

# Nebula
add_subdirectory(Nebula)

add_subdirectory(Projects)