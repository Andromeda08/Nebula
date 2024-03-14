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
    enum class AmbientOcclusionMode
    {
        eSSAO,
        eRTAO,
        eFFXCacao,
    };

    inline std::string to_string(const AmbientOcclusionMode shadow_mode) noexcept
    {
        using enum AmbientOcclusionMode;
        switch (shadow_mode)
        {
            case eSSAO:     return "SSAO";
            case eRTAO:     return "RTAO";
            case eFFXCacao: return "FFX Cacao";
            default:        return "Unknown";
        }
    }

    class AmbientOcclusion : public Node
    {
    public:
        struct Configuration : public NodeConfiguration
        {
            int selected_mode_idx {0};

            void render() override;
            bool validate() override { return true; }
            ~Configuration() override = default;

        private:
            std::vector<std::string> m_modes = {
                to_string(AmbientOcclusionMode::eSSAO), to_string(AmbientOcclusionMode::eRTAO),
                to_string(AmbientOcclusionMode::eFFXCacao),
            };
        };

        explicit AmbientOcclusion(const std::shared_ptr<Configuration>& configuration,const std::shared_ptr<nvk::Device>& device)
        : Node("AmbientOcclusion", NodeType::eAmbientOcclusion)
        , m_configuration(*configuration), m_device(device) {}

        ~AmbientOcclusion() override = default;

        void initialize() override {}

        void execute(const vk::CommandBuffer& command_buffer) override {}

        void update() override {}

    private:
        const Configuration          m_configuration;
        std::shared_ptr<nvk::Device> m_device;

        static constexpr const char* s_output       = "AO Buffer";
        static constexpr const char* s_position     = "Position Buffer";
        static constexpr const char* s_normal       = "Normal Buffer";
        static constexpr const char* s_depth        = "Depth Buffer";
        static constexpr const char* s_scene_data   = "Scene Data";

        nrg_decl_resource_requirements();
        nrg_def_get_resource_claims();
    };
}