cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

IF(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
ENDIF()

set("nebula" "../../Nebula/nebula")

add_executable(VulkanPlayground main.cpp VkPlayground.hpp VkPlayground.cpp)
target_link_libraries(VulkanPlayground PUBLIC nebula)
target_include_directories(VulkanPlayground PUBLIC "${nebula}")