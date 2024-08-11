#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace vk
{
    class CommandBuffer;
}

namespace Nebula::nrg
{
    class Node;
    class Resource;

    class RenderPath
    {
    public:
        RenderPath(std::vector<std::shared_ptr<Node>>&& nodes, std::map<std::string, std::shared_ptr<Resource>>&& resources)
        : m_nodes(std::move(nodes)), m_resources(std::move(resources))
        {
        }

        void execute(const vk::CommandBuffer& command_buffer);

    private:

        void initialize(const vk::CommandBuffer& command_buffer);

        bool                                             m_initialized {false};
        std::vector<std::shared_ptr<Node>>               m_nodes;
        std::map<std::string, std::shared_ptr<Resource>> m_resources;

        friend class GraphEditor;
    };
}