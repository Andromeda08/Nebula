#include <memory>

#include "nvk/Context.hpp"
#include "scene/Scene.hpp"
#include "wsi/Window.hpp"

using namespace Eikon;
using namespace Nebula;

int main()
{
    auto wnd_create_info = wsi::WindowCreateInfo()
        .set_size({ 1920, 1080 })
        .set_fullscreen(false)
        .set_title("Eikon");
    auto window = std::make_shared<wsi::Window>(wnd_create_info);

    std::vector instance_extensions { VK_KHR_SURFACE_EXTENSION_NAME };
    std::vector instance_layers { "VK_LAYER_KHRONOS_validation" };
    auto ctx_create_info = nvk::ContextCreateInfo()
        .set_instance_extensions(instance_extensions)
        .set_instance_layers(instance_layers)
        .set_debug_mode(true)
        .set_validation_layers(true)
        .set_ray_tracing_features(false)
        .set_mesh_shader_features(false)
        .set_window(window);
    auto context = std::make_shared<nvk::Context>(ctx_create_info);

    auto scene = std::make_unique<glTFScene>("Bistro.glb", context->device(), context->command_pool());
}