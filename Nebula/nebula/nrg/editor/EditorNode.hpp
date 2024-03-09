#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <nmath/Utility.hpp>
#include <nmath/graph/Vertex.hpp>
#include <nrg/common/NodeType.hpp>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    struct NodeConfiguration
    {
        virtual void render() { /* no-op default */ }
    };

    class EditorNode : public nmath::graph::Vertex
    {
    public:
        EditorNode(NodeType node_type,
                   const std::string& name,
                   const NodeColors& colors,
                   const std::vector<ResourceClaim>& resources,
                   std::shared_ptr<NodeConfiguration> configuration = nullptr);

        void render() const;

        ResourceClaim& get_resource(int32_t id);

        ResourceClaim& get_resource(const std::string& name);

        int32_t id() const { return m_id; }

        NodeType type() const { return m_type; }

    private:
        NodeColors                         m_colors {};
        std::shared_ptr<NodeConfiguration> m_config;
        int32_t                            m_id {nmath::rand()};
        std::string                        m_name {"Unknown Node"};
        std::vector<ResourceClaim>         m_resource_claims {};
        NodeType                           m_type {NodeType::eUnknown};
    };
}