cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Vulkan REQUIRED)

# nvk
set (Nebula_main "nebula")

set("3RD" "../../ThirdParty")
set("IMGUI_DIR" "${3RD}/imgui")
set("IMNODES_DIR" "${3RD}/imnodes")
set("NLOG" "../nlog")

add_executable(${Nebula_main}
    ${3RD}/cy/cyCore.h ${3RD}/cy/cyHairFile.h
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
    ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui.h ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMNODES_DIR}/imnodes.cpp

    main.cpp

    ncommon/Size2D.hpp

    wsi/Window.hpp wsi/Window.cpp
    wsi/WindowCreateInfo.hpp

    napp/AppConfig.hpp napp/AppConfig.cpp
    napp/Application.hpp napp/Application.cpp

    ngui/GUI.hpp ngui/GUI.cpp

    nmath/Transform.hpp
    nmath/Utility.hpp nmath/Utility.cpp
    nmath/graph/Vertex.hpp nmath/graph/Vertex.cpp

    nscene/Light.hpp
    nscene/Camera.hpp nscene/Camera.cpp
    nscene/Object.hpp
    nscene/Scene.hpp nscene/Scene.cpp
    nscene/Vertex.hpp

    nscene/geometry/Geometry.hpp nscene/geometry/Geometry.cpp
    nscene/geometry/Mesh.hpp nscene/geometry/Mesh.cpp
    nscene/geometry/primitives/Cube.hpp nscene/geometry/primitives/Cube.cpp
    nscene/geometry/primitives/Sphere.hpp nscene/geometry/primitives/Sphere.cpp

    nhair/HairModel.hpp nhair/HairModel.cpp
    nhair/HairRenderer.hpp nhair/HairRenderer.cpp

    nrg/common/Context.hpp
    nrg/common/Node.hpp nrg/common/Node.cpp
    nrg/common/NodeConfiguration.hpp
    nrg/common/Resource.hpp
    nrg/common/NodeType.hpp
    nrg/common/RenderPath.hpp
    nrg/common/ResourceClaim.hpp
    nrg/common/ResourceSpecification.hpp
    nrg/common/ResourceTraits.hpp

    nrg/editor/Edge.hpp
    nrg/editor/Graph.hpp
    nrg/editor/GraphEditor.hpp nrg/editor/GraphEditor.cpp
    nrg/editor/EditorNode.hpp nrg/editor/EditorNode.cpp
    nrg/editor/EditorNodeFactory.hpp nrg/editor/EditorNodeFactory.cpp

    nrg/nodes/Nodes.hpp
    nrg/nodes/AntiAliasing.hpp nrg/nodes/AntiAliasing.cpp
    nrg/nodes/SceneDataProvider.hpp nrg/nodes/SceneDataProvider.cpp
)

target_link_libraries(${Nebula_main} PRIVATE ${Vulkan_LIBRARIES} nvk glm glfw nlohmann_json::nlohmann_json stduuid)
target_include_directories(${Nebula_main} PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    "../nvk"
    "${3RD}/cy"
    "${3RD}/glm"
    "${3RD}/glfw"
    "${3RD}/json"
    "${3RD}/stduuid/include"
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
    "${IMNODES_DIR}"
    "${NLOG}")
target_compile_definitions (${Nebula_main} PRIVATE
    NBL_DEBUG
    -DImTextureID=ImU64
    IMGUI_DEFINE_MATH_OPERATORS
    GLFW_INCLUDE_VULKAN
    UUID_SYSTEM_GENERATOR
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    NLOG_COLORED NLOG_PRINT_EXCEPTIONS)
