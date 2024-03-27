cmake_minimum_required(VERSION 3.23)
project("Nebula")

# External Dependencies
find_package(Vulkan REQUIRED)

add_subdirectory(ThirdParty/glm)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_ENABLE_EXPERIMENTAL)

add_subdirectory(ThirdParty/glfw)

add_subdirectory(ThirdParty/json)

add_subdirectory(ThirdParty/stduuid)

# Nebula
add_subdirectory(Nebula)