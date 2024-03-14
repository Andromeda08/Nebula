#include "DeferredLighting.hpp"
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    nrg_def_resource_requirements(DeferredLighting, ({
        std::make_shared<ImageRequirement>(s_position, ResourceUsage::eInput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_normal, ResourceUsage::eInput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_albedo, ResourceUsage::eInput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_ao, ResourceUsage::eInput, ResourceType::eImage, vk::Format::eR32Sfloat),
        std::make_shared<ImageRequirement>(s_shadows, ResourceUsage::eInput, ResourceType::eImage, vk::Format::eR32Sfloat),
        std::make_shared<Requirement>(s_scene_data, ResourceUsage::eInput, ResourceType::eSceneData),
        std::make_shared<ImageRequirement>(s_output, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
    }));

    void DeferredLighting::Configuration::render()
    {
        ImGui::PushItemWidth(128);
        {
            ImGui::Checkbox("Use Ambient Occlusion", &use_ambient_occlusion);

            if (ImGui::BeginCombo("Shadows", m_shadow_modes[shadow_mode_idx].c_str()))
            {
                for (int n = 0; n < m_shadow_modes.size(); n++)
                {
                    const bool is_selected = (shadow_mode_idx == n);
                    if (ImGui::Selectable(m_shadow_modes[n].c_str(), is_selected))
                        shadow_mode_idx = n;

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

        }
        ImGui::PopItemWidth();
    }
}