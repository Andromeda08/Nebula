#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vulkan/vulkan.hpp>
#include <nrg/common/Node.hpp>
#include <nrg/common/NodeConfiguration.hpp>
#include <nrg/common/NodeTraits.hpp>
#include <nrg/common/ResourceClaim.hpp>
#include <nvk/Descriptor.hpp>
#include <nvk/Device.hpp>
#include <nvk/render/Pipeline.hpp>

namespace Nebula::nrg
{
    enum class AntiAliasingMode
    {
        eDisabled,
        eFXAA,
        eFSR2,
        eUnknown,
    };

    class AntiAliasing : public Node
    {
    public:
        struct Configuration : public NodeConfiguration
        {
            AntiAliasingMode m_mode {AntiAliasingMode::eFXAA};

            void render() override;
            bool validate() override;

            ~Configuration() override = default;
        };

        struct alignas(glm::vec4) PushConstant
        {
            glm::vec2 resolution_rcp;
        };

        struct Renderer
        {
            static constexpr const char*     s_vert_shader = "fullscreen_quad.vert.hlsl.spv";
            static constexpr const char*     s_frag_shader = "fxaa.hlsl.spv";

            std::shared_ptr<nvk::Descriptor> descriptor;
            std::shared_ptr<nvk::Pipeline>   pipeline;
            vk::RenderPass                   render_pass;
            std::array<vk::ClearValue, 1>    clear_values {vk::ClearValue{{0.0f, 0.0f, 0.0f, 1.0f}}};
            std::vector<vk::Framebuffer>     framebuffers;
            glm::vec2                        resolution_rcp; // 1/res
        };

        AntiAliasing(const std::shared_ptr<Configuration>& configuration,
                     const std::shared_ptr<nvk::Device>& device);

        void initialize() override;

        void execute(const vk::CommandBuffer &command_buffer) override;

        void update() override;

    private:
        static constexpr const char* s_input_name  = "AA Input";
        static constexpr const char* s_output_name = "AA Output";

        const Configuration          m_configuration;
        std::shared_ptr<nvk::Device> m_device;
        std::unique_ptr<Renderer>    m_renderer;

        nrg_decl_resource_requirements();
        nrg_def_get_resource_claims();
    };

    inline std::string to_string(const AntiAliasingMode aa_mode)
    {
        using enum AntiAliasingMode;
        switch (aa_mode)
        {
            case eDisabled: return "Disabled";
            case eFXAA:     return "FXAA";
            case eFSR2:     return "FSR2";
            default:        return "Unknown";
        }
    }
}