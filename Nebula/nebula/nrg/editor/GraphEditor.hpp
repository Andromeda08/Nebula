#pragma once

#include <nlohmann/json.hpp>
#include <nlog/nlog.hpp>
#include <nrg/common/Context.hpp>
#include <nrg/editor/Edge.hpp>
#include <nrg/editor/EditorNode.hpp>
#include <nrg/editor/EditorNodeFactory.hpp>
#include <nrg/editor/Graph.hpp>

namespace Nebula::nrg
{
    class GraphEditor
    {
    public:
        explicit GraphEditor(const std::shared_ptr<Context>& context);

        void render();

    private:
        void _load_editor_config();
        void _process_node_colors(const nlohmann::json& data);
        void _process_resource_colors(const nlohmann::json& data);

        void _add_defaults();

        void _add_node(NodeType node_type);
        void _erase_edge(int32_t edge_id);

        void _handle_compile();
        bool _handle_connection();
        void _handle_edge_deletion();
        void _handle_reset_graph();

        void _render_menu_bar();
        void _render_node_editor();

        static constexpr const char*       s_config_file = "nrg_config.json";

        std::map<NodeType, NodeColors>     m_node_colors;
        std::map<ResourceType, glm::ivec4> m_resource_colors;

        std::vector<NodeType>              m_enabled_node_types {};
        Graph                              m_graph;
        bool                               m_has_present_node {false};
        bool                               m_has_scene_data_node {false};
        bool                               m_log_stdout {true};
        std::unique_ptr<nlog::Logger>      m_logger;
        std::unique_ptr<EditorNodeFactory> m_node_factory;
        std::shared_ptr<Context>           m_context;
    };
}