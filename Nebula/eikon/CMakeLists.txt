cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

IF(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
ENDIF()

find_package(Vulkan REQUIRED)

# nvk
set (Eikon "eikon")

set("3RD" "../../ThirdParty")
set("IMGUI_DIR" "${3RD}/imgui")
set("IMNODES_DIR" "${3RD}/imnodes")

add_executable(${Eikon}
    scene/Scene.hpp
    scene/tinygltf.impl.cpp
    main.cpp
    wsi/Window.cpp wsi/Window.hpp
    Eikon.hpp
    base/UserInterface.hpp
    render/ImGuiRenderer.hpp
    base/UIItem.hpp
    render/ImGuiRenderer.cpp
    base/UserInterface.cpp
    base/ui/DebugStatsItem.hpp
    base/ui/DebugStatsItem.cpp
    base/Application.hpp
    base/Application.cpp
    base/config/Configuration.hpp
    base/Timer.hpp
)

target_link_libraries(${Eikon} PUBLIC ${Vulkan_LIBRARIES} nvk glm glfw nlohmann_json::nlohmann_json stduuid fmt::fmt)
target_include_directories(${Eikon} PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    "../nvk/include"
    "${3RD}/cy"
    "${3RD}/fmt/include"
    "${3RD}/glm"
    "${3RD}/glfw"
    "${3RD}/json"
    "${3RD}/stb"
    "${3RD}/stduuid/include"
    "${3RD}/tinygltf"
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
    "${IMNODES_DIR}"
)
target_compile_definitions (${Eikon} PUBLIC
    -DImTextureID=ImU64
    IMGUI_DEFINE_MATH_OPERATORS
    GLFW_INCLUDE_VULKAN
    UUID_SYSTEM_GENERATOR
)
