#include "EditorNode.hpp"

#include <format>
#include <stdexcept>
#include <utility>
#include <imgui.h>
#include <imnodes.h>

namespace Nebula::nrg
{
    EditorNode::EditorNode(NodeType node_type, const std::string& name, const NodeColors& colors,
                           const std::vector<ResourceClaim>& resources, std::shared_ptr<NodeConfiguration> configuration)
    : nmath::graph::Vertex(name), m_type(node_type), m_name(name), m_colors(colors), m_resource_claims(resources), m_config(std::move(configuration))
    {
    }

    void EditorNode::render() const
    {
        m_colors.push_color_styles();
        {
            ImNodes::BeginNode(m_id);
            {
                ImNodes::BeginNodeTitleBar(); {
                    ImGui::TextUnformatted(m_name.c_str());
                } ImNodes::EndNodeTitleBar();

                if (m_config) m_config->render();

                for (const auto& resource : m_resource_claims)
                {
                    if (resource.usage() == ResourceUsage::eUnknown) continue;

                    const int32_t attribute_id = resource.id;
                    ImNodes::PushColorStyle(ImNodesCol_Pin, TO_IM_COL32(get_resource_color(resource.type())));

                    switch (resource.usage())
                    {
                        case ResourceUsage::eInput:
                            ImNodes::BeginInputAttribute(attribute_id, ImNodesPinShape_CircleFilled);
                            break;
                        case ResourceUsage::eOutput:
                            ImNodes::BeginOutputAttribute(attribute_id, ImNodesPinShape_CircleFilled);
                            break;
                        default:
                            break;
                    }

                    ImGui::Text("%s", resource.name().c_str());

                    switch (resource.usage())
                    {
                        case ResourceUsage::eInput:
                            ImNodes::EndInputAttribute();
                            break;
                        case ResourceUsage::eOutput:
                            ImNodes::EndOutputAttribute();
                            break;
                        default:
                            break;
                    }

                    ImNodes::PopColorStyle();
                }
            }
            ImNodes::EndNode();
        }
        NodeColors::pop_color_styles();
    }

    ResourceClaim& EditorNode::get_resource(int32_t id)
    {
        for (auto& resource : m_resource_claims)
        {
            if (resource.id == id) return resource;
        }
        throw std::runtime_error(std::format("No Resource found with ID {}", id));
    }

    ResourceClaim& EditorNode::get_resource(const std::string& name)
    {
        for (auto& resource : m_resource_claims)
        {
            if (resource.name() == name) return resource;
        }
        throw std::runtime_error(std::format("No Resource found with the name {}", name));
    }
}