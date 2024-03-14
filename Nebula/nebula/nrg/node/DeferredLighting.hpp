#pragma once

#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <nrg/common/Node.hpp>
#include <nrg/common/NodeConfiguration.hpp>
#include <nrg/common/NodeTraits.hpp>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/resource/Requirement.hpp>
#include <nvk/Device.hpp>

namespace Nebula::nrg
{
    enum class ShadowMode
    {
        eNone,
        eRaytraced,
        eShadowMaps
    };

    inline std::string to_string(const ShadowMode shadow_mode) noexcept
    {
        using enum ShadowMode;
        switch (shadow_mode)
        {
            case eNone:         return "None";
            case eRaytraced:    return "Raytraced";
            case eShadowMaps:   return "ShadowMaps";
            default:            return "Unknown";
        }
    }

    inline ShadowMode from_string(const std::string& string)
    {
        using enum ShadowMode;
        if (string == "None")       return eNone;
        if (string == "Raytraced")  return eRaytraced;
        if (string == "ShadowMaps") return eShadowMaps;
        throw std::runtime_error("Unexpected ShadowMode string");
    }

    class DeferredLighting : public Node
    {
    public:
        struct Configuration : public NodeConfiguration
        {
            int  shadow_mode_idx       {0};
            bool use_ambient_occlusion {false};

            void render() override;
            bool validate() override { return true; }
            ~Configuration() override = default;

        private:
            std::vector<std::string> m_shadow_modes = {
                to_string(ShadowMode::eNone), to_string(ShadowMode::eRaytraced),
                to_string(ShadowMode::eShadowMaps),
            };
        };

        DeferredLighting(const std::shared_ptr<Configuration>& configuration, const std::shared_ptr<nvk::Device>& device)
        : Node("DeferredLighting", NodeType::eDeferredLighting)
        , m_configuration(*configuration), m_device(device) {}

        ~DeferredLighting() override = default;

        void initialize() override {}

        void execute(const vk::CommandBuffer& command_buffer) override {}

        void update() override {}

    private:
        const Configuration          m_configuration;
        std::shared_ptr<nvk::Device> m_device;

        static constexpr const char* s_output     = "Output Image";
        static constexpr const char* s_position   = "Position Buffer";
        static constexpr const char* s_normal     = "Normal Buffer";
        static constexpr const char* s_albedo     = "Albedo Buffer";
        static constexpr const char* s_ao         = "Ambient Occlusion";
        static constexpr const char* s_shadows    = "Shadow Maps";
        static constexpr const char* s_scene_data = "Scene Data";

        nrg_decl_resource_requirements();
        nrg_def_get_resource_claims();
    };
}