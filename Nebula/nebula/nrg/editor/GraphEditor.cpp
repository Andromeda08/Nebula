#include "GraphEditor.hpp"

#include <algorithm>
#include <format>
#include <iostream>
#include <stdexcept>
#include <imgui.h>
#include <imnodes.h>

namespace Nebula::nrg
{
    GraphEditor::GraphEditor(const std::shared_ptr<Context>& context)
    : m_context(context)
    {
        m_node_factory = std::make_shared<EditorNodeFactory>(m_context);
        _add_defaults();
    }

    void GraphEditor::render()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 16.0f, 16.0f });
        ImGui::Begin("RenderGraph Editor", nullptr, ImGuiWindowFlags_MenuBar);
        {
            _render_menu_bar();
            _render_node_editor();
            _handle_connection();
            _handle_edge_deletion();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void GraphEditor::_add_defaults()
    {
        for (const std::vector to_create = { NodeType::eSceneDataProvider };
             const NodeType t : to_create)
        {
            switch (t)
            {
                case NodeType::eSceneDataProvider:
                    m_has_scene_data_node = true;
                    break;
                case NodeType::ePresent:
                    m_has_present_node = true;
                    break;
                default:
                    break;
            }

            _add_node(t);
        }
    }

    void GraphEditor::_add_node(NodeType node_type)
    {
        const auto node = m_node_factory->create(node_type);
        m_graph.nodes.insert({ node->id(), node });
    }

    void GraphEditor::_erase_edge(int32_t edge_id)
    {
        auto& edges = m_graph.edges;
        auto& nodes = m_graph.nodes;
        if (const auto edge = std::ranges::find_if(edges, [edge_id](const auto& e){ return e.id == edge_id; });
            edge != edges.end())
        {
            const auto& s_node = nodes[edge->start.node_id];
            auto& s_attr = s_node->get_resource(edge->start.resource_id);

            const auto& e_node = nodes[edge->end.node_id];
            auto& e_attr = e_node->get_resource(edge->end.resource_id);
            e_attr.input_connected = false;

            EditorNode::delete_directed_edge(s_node, e_node);

            edges.erase(edge);

            std::string message = std::format(
                R"([Editor] Deleting link: "{}" ("{}") --> "{}" ("{}"))",
                s_node->name(), s_attr.name, e_node->name(), e_attr.name);

            m_logs.push_back(message);
            if (m_log_stdout)
            {
                std::cout << message << std::endl;
            }
        }
    }

    void GraphEditor::_handle_compile() {}

    bool GraphEditor::_handle_connection()
    {
        auto& edges = m_graph.edges;
        auto& nodes = m_graph.nodes;

        int32_t start_node, start_attr;
        int32_t end_node, end_attr;

        if (ImNodes::IsLinkCreated(&start_node, &start_attr, &end_node, &end_attr)) {
            try {
                const auto& attr = nodes[start_node]->get_resource(start_attr);
            }
            catch (const std::runtime_error& ex) {
                std::swap(start_node, end_node);
                std::swap(start_attr, end_attr);
            }

            const auto& s_node = nodes[start_node];
            auto& s_attr = s_node->get_resource(start_attr);

            const auto& e_node = nodes[end_node];
            auto& e_attr = e_node->get_resource(end_attr);

            const auto edge_exists = std::ranges::any_of(edges, [s_attr, e_attr](const Edge& edge){
                return edge.start.resource_id == s_attr.id && edge.end.resource_id == e_attr.id;
            });

            std::string message;
            if (e_attr.input_connected)
            {
                message = std::format(R"([Editor] Error: The attribute "{}" of "{}" already has an input attached.)", e_attr.name, e_node->name());
                m_logs.push_back(message);

                if (m_log_stdout)
                {
                    std::cout << message << std::endl;
                }

                return false;
            }

            if (edge_exists)
            {
                message = std::format(R"([Editor] Error: The attributes "{}" and "{}" are already connected.)", s_attr.name, e_attr.name);
                m_logs.push_back(message);

                if (m_log_stdout)
                {
                    std::cout << message << std::endl;
                }

                return false;
            }

            if (s_attr.type != e_attr.type)
            {
                message = std::format(R"([Editor] Error: Type of attribute "{}" is not compatible with "{}".)", s_attr.name, e_attr.name);
                m_logs.push_back(message);

                if (m_log_stdout)
                {
                    std::cout << message << std::endl;
                }

                return false;
            }

            EditorNode::make_directed_edge(s_node, e_node);
            edges.emplace_back(*s_node, s_attr, *e_node, e_attr, s_attr.type);
            e_attr.input_connected = true;

            message = std::format(
                R"([Editor] Connecting: "{}" ("{}") --> "{}" ("{}"))",
                s_node->name(), s_attr.name, e_node->name(), e_attr.name);
            m_logs.push_back(message);

            if (m_log_stdout)
            {
                std::cout << message << std::endl;
            }
        }

        return true;
    }

    void GraphEditor::_handle_edge_deletion()
    {
        if (const int32_t n_selected = ImNodes::NumSelectedLinks();
            n_selected > 0 && ImGui::IsKeyReleased(ImGuiKey_X))
        {
            std::vector<int32_t> selected_links;
            selected_links.resize(static_cast<size_t>(n_selected));
            ImNodes::GetSelectedLinks(selected_links.data());
            for (const int32_t edge_id : selected_links)
            {
                _erase_edge(edge_id);
            }
        }
    }

    void GraphEditor::_handle_reset_graph()
    {
        m_graph.reset();
        _add_defaults();

        std::string message = "[Editor] Graph editor has been reset";
        m_logs.push_back(message);

        if (m_log_stdout)
        {
            std::cout << message << std::endl;
        }
    }

    void GraphEditor::_render_menu_bar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Add Node"))
            {
                for (const auto& node_type : m_enabled_node_types)
                {
                    if (ImGui::MenuItem(to_string(node_type).c_str()))
                    {
                        _add_node(node_type);
                    }
                    if (!m_has_scene_data_node)
                    {
                        if (ImGui::BeginMenu("Scene Data Provider"))
                        {
                            _add_node(NodeType::eSceneDataProvider);
                        }
                    }
                    if (!m_has_present_node)
                    {
                        if (ImGui::BeginMenu("Present"))
                        {
                            _add_node(NodeType::ePresent);
                        }
                    }
                }

                ImGui::EndMenu();
            }

            std::string select_scene_text = std::format(
                "Select Scene (Active: \"{}\")",
                m_context->get_selected_scene()->name());

            if (ImGui::BeginMenu(select_scene_text.c_str()))
            {
                for (int32_t i = 0; i < m_context->m_scene_list.size(); i++)
                {
                    if (ImGui::MenuItem(m_context->m_scene_list[i]->name().c_str()))
                    {
                        m_context->m_selected_scene = i;
                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::Button("Compile"))
            {
                _handle_compile();
            }

            if (ImGui::Button("Reset"))
            {
                _handle_reset_graph();
            }

            ImGui::EndMenuBar();
        }
    }

    void GraphEditor::_render_node_editor()
    {
        ImNodes::BeginNodeEditor();
        {
            ImNodes::PushStyleVar(ImNodesStyleVar_PinCircleRadius, 4.0f);
            ImNodes::PushStyleVar(ImNodesStyleVar_LinkThickness, 3.0f);
            for (const auto& [id, node] : m_graph.nodes)
            {
                node->render();
            }
            for (const auto& edge : m_graph.edges)
            {
                const auto link_color = get_resource_color(edge.attr_type);
                ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(link_color.r, link_color.g, link_color.b, 255));
                ImNodes::Link(edge.id, edge.start.resource_id, edge.end.resource_id);
                ImNodes::PopColorStyle();
            }
        }
        ImNodes::EndNodeEditor();
    }
}
