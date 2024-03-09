#pragma once

#include <cstdint>
#include <string>
#include <nmath/Utility.hpp>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/editor/EditorNode.hpp>

namespace Nebula::nrg
{
    class Edge
    {
        struct Point
        {
            int32_t     node_id,   resource_id;
            std::string node_name, resource_name;

            Point(const EditorNode& editor_node, const ResourceClaim& resource)
            : node_id(editor_node.id()), resource_id(resource.id)
            , node_name(editor_node.name()), resource_name(resource.name)
            {
            }
        };

    public:
        Edge(const EditorNode& node_start, const ResourceClaim& resource_start,
             const EditorNode& node_end, const ResourceClaim& resource_end,
             const ResourceType attribute_type)
        : start(node_start, resource_start), end(node_end, resource_end), attr_type(attribute_type)
        {
        }

        int32_t      id {nmath::rand()};
        Point        start;
        Point        end;
        ResourceType attr_type {ResourceType::eUnknown};
    };
}