#include "NodeFactory.hpp"
#include <iostream>
#include <nrg/node/Nodes.hpp>

namespace Nebula::nrg
{
    std::shared_ptr<Node> NodeFactory::create(const std::shared_ptr<EditorNode>& editor_node)
    {
        switch (editor_node->type())
        {
            case NodeType::eSceneDataProvider: {
                return std::make_shared<SceneDataProvider>();
            }
            case NodeType::eGBuffer: {
                return std::make_shared<GBuffer>(m_context);
            }
            default:
                std::cout << std::format("Creation of NodeType \"{}\" not implemented", to_string(editor_node->type())) << std::endl;
                return nullptr;
        }
    }
}