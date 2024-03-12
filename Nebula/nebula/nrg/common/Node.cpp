#include "Node.hpp"

namespace Nebula::nrg
{
    Node::Node(std::string name, const NodeType node_type)
    : m_name(std::move(name)), m_type(node_type)
    {
    }

    void Node::set_common(const Node::Common& common)
    {
        m_common = common;
    }

    const std::string& Node::name() const
    {
        return m_name;
    }

    std::map<std::string, std::shared_ptr<Resource>>& Node::resources()
    {
        return m_resources;
    }

    bool Node::set_resource(const std::string& key, const std::shared_ptr<Resource>& resource)
    {
        if (!m_resources.contains(key))
        {
            m_resources.insert({key, resource});
            return true;
        }

        m_resources[key] = resource;
        return true;
    }
}