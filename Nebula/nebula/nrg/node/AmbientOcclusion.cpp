#include "AmbientOcclusion.hpp"
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    nrg_def_resource_requirements(AmbientOcclusion, ({
        std::make_shared<ImageRequirement>(s_position, ResourceUsage::eInput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_normal, ResourceUsage::eInput, ResourceType::eImage, vk::Format::eR32G32B32A32Sfloat),
        std::make_shared<ImageRequirement>(s_depth, ResourceUsage::eInput, ResourceType::eImage, vk::Format::eD32Sfloat),
        std::make_shared<Requirement>(s_scene_data, ResourceUsage::eInput, ResourceType::eSceneData),
        std::make_shared<ImageRequirement>(s_output, ResourceUsage::eOutput, ResourceType::eImage, vk::Format::eR32Sfloat)
    }));

    void AmbientOcclusion::Configuration::render()
    {
        ImGui::PushItemWidth(128);
        {
            if (ImGui::BeginCombo("Method", m_modes[selected_mode_idx].c_str()))
            {
                for (int n = 0; n < m_modes.size(); n++)
                {
                    const bool is_selected = (selected_mode_idx == n);
                    if (ImGui::Selectable(m_modes[n].c_str(), is_selected))
                        selected_mode_idx = n;

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

        }
        ImGui::PopItemWidth();
    }
}