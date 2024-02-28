cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Vulkan REQUIRED)

# nvk
set (Nebula_main "nebula")

set("3RD" "../../ThirdParty")
set("IMGUI_DIR" "${3RD}/imgui")
set("IMNODES_DIR" "${3RD}/imnodes")

add_library(${Nebula_main}
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
    ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui.h ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMNODES_DIR}/imnodes.cpp

    common/Size2D.hpp

    wsi/Window.hpp wsi/Window.cpp
    wsi/WindowCreateInfo.hpp

    napp/AppConfig.hpp
    napp/Application.hpp napp/Application.cpp

    ngui/GUI.hpp ngui/GUI.cpp

    nmath/Transform.hpp
    nmath/Utility.hpp nmath/Utility.cpp

    nscene/Light.hpp
    nscene/Camera.hpp nscene/Camera.cpp
    nscene/Object.hpp
    nscene/Scene.hpp nscene/Scene.cpp
    nscene/Vertex.hpp

    nscene/geometry/Geometry.hpp nscene/geometry/Geometry.cpp
    nscene/geometry/Mesh.hpp nscene/geometry/Mesh.cpp
    nscene/geometry/primitives/Cube.hpp nscene/geometry/primitives/Cube.cpp
    nscene/geometry/primitives/Sphere.hpp nscene/geometry/primitives/Sphere.cpp
)

target_link_libraries(${Nebula_main} PRIVATE ${Vulkan_LIBRARIES} nvk glm glfw)
target_include_directories(${Nebula_main} PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    "../nvk"
    "${3RD}/glm"
    "${3RD}/glfw"
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
    "${IMNODES_DIR}")
target_compile_definitions (${Nebula_main} PRIVATE
    NBL_DEBUG
    GLFW_INCLUDE_VULKAN
    IMGUI_DEFINE_MATH_OPERATORS
    -DImTextureID=ImU64)