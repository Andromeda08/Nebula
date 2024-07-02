#include "Utility.hpp"
#include "Utilities.hpp"

namespace Nebula::nvk
{
    VkBool32 VKAPI_CALL
    Utility::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
                            const VkDebugUtilsMessengerCallbackDataEXT* p_data, void* p_user)
    {
        if (!p_data) return vk::False;
        if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            print_verbose("{}", p_data->pMessage);
        }
        return vk::False;
    }
}
