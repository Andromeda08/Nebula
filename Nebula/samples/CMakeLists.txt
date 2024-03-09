# Hello Triangle
# add_executable("nbl_hello_world" nvk_hello_triangle.cpp)

# target_link_libraries     ("nbl_hello_world" PRIVATE ${Vulkan_LIBRARIES} glm glfw nvk nebula)
# target_include_directories("nbl_hello_world" PRIVATE ${Vulkan_INCLUDE_DIRS} "../nvk" "../nebula")
# target_compile_definitions("nbl_hello_world" PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

#add_executable("NebulaApplication" nbl_application.cpp)
#
#target_link_libraries     ("NebulaApplication" PRIVATE ${Vulkan_LIBRARIES} glm glfw nvk nebula)
#target_include_directories("NebulaApplication" PRIVATE ${Vulkan_INCLUDE_DIRS} "../nvk" "../nebula")
#target_compile_definitions("NebulaApplication" PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)