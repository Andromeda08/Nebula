#pragma once

#include <bitset>
#include <format>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include <nrg/editor/EditorNode.hpp>
#include <nrg/editor/Graph.hpp>

namespace Nebula::nrg
{
    using node_ptr = std::shared_ptr<EditorNode>;

    static constexpr bool is_optimizable_type(const ResourceType resource_type)
    {
        return resource_type == ResourceType::eImage;
    }

    struct consumer_info
    {
        int32_t         node_id {};     // ID of the Consumer Node
        int32_t         node_idx {};    // [Need?] Index of the Consumer Node in the execution order
        std::string     node_name;      // [Need?] Name of the Consumer Node
        int32_t         resource_id;    // ID of the ResourceClaim at the Consumer Node
        std::string     resource_name;  // [Need?] Name of the Resource at the Consumer Node
        ResourceUsage   usage;          // Usage of the Resource at the Consumer Node
        node_ptr        node;           // Pointer to the Consumer Node
    };

    struct resource_info
    {
        int32_t       origin_node_id {};
        int32_t       origin_node_idx {};
        std::string   origin_node_name {};
        int32_t       origin_res_id {};
        std::string   origin_res_name;
        ResourceType  type {ResourceType::eUnknown};
        ResourceUsage usage {ResourceUsage::eUnknown};
        bool          optimizable {false};
        ResourceClaim claim;
        std::vector<consumer_info> consumers;

        static resource_info create_from(const EditorNode& node, const ResourceClaim& claim, size_t i)
        {
            return {
                .origin_node_id = node.id(),
                .origin_node_idx = static_cast<int32_t>(i),
                .origin_node_name = node.name(),
                .origin_res_id = claim.id,
                .origin_res_name = claim.name(),
                .type = claim.type(),
                .usage = claim.usage(),
                .optimizable = is_optimizable_type(claim.type()),
                .claim = claim,
                .consumers = {},
            };
        }
    };

    struct usage_point
    {
        int32_t       point {};
        int32_t       user_res_id {-1};
        std::string   used_as;
        int32_t       user_node_id {-1};
        std::string   used_by;
        ResourceUsage usage {ResourceUsage::eUnknown};

        usage_point() = default;

        explicit usage_point(const resource_info& iri)
        {
            point        = iri.origin_node_idx;
            user_res_id  = iri.origin_res_id;
            used_as      = iri.origin_res_name;
            user_node_id = iri.origin_node_id;
            used_by      = iri.origin_node_name;
            usage        = iri.usage;
        }

        explicit usage_point(const consumer_info& consumer)
        {
            point        = consumer.node_idx;
            user_res_id  = consumer.resource_id;
            used_as      = consumer.resource_name;
            user_node_id = consumer.node_id;
            used_by      = consumer.node_name;
            usage        = consumer.usage;
        }
    };

    inline bool operator<(const usage_point& lhs, const usage_point& rhs)
    {
        return lhs.point < rhs.point;
    }

    inline bool operator==(const usage_point& lhs, const usage_point& rhs)
    {
        return lhs.point == rhs.point;
    }

    struct Range
    {
        int32_t start;
        int32_t end;

        explicit Range(const std::set<usage_point>& points)
        {
            const auto min = std::min_element(std::begin(points), std::end(points));
            const auto max = std::max_element(std::begin(points), std::end(points));

            start = min->point;
            end = max->point;

            validate();
        }

        Range(int32_t a, int32_t b): start(a), end(b)
        {
            validate();
        }

        bool overlaps(const Range& other) const
        {
            return std::max(start, other.start) <= std::min(end, other.end);
        }

    private:
        void validate() const
        {
            if (start > end)
            {
                throw std::runtime_error(std::format("[Error] Range starting point {} is greater than the end point {}", start, end));
            }
        }
    };

    struct OptimizerResource
    {
        int32_t               id;
        std::set<usage_point> usage_points;
        resource_info         original_info;
        ResourceType          type;

        // Ensure image format compatibility
        vk::Format            format;
        vk::ImageUsageFlags   usage_flags;

        Range get_usage_range() const { return Range(usage_points); }

        bool insert_usage_points(const std::set<usage_point>& points)
        {
            // Validation for occupied usage points
            std::vector<usage_point> intersection;
            std::set_intersection(std::begin(usage_points), std::end(usage_points), std::begin(points), std::end(points), std::back_inserter(intersection));

            if (!intersection.empty())
            {
                return false;
            }

            for (const auto& point: points)
            {
                usage_points.insert(point);
            }

            return true;
        }
    };

    struct ResourceOptimizerResult
    {
        std::vector<OptimizerResource> resources;

        // Metadata
        std::vector<resource_info> original_resources;
        int32_t non_optimizable_count {0};
        int32_t optimized_resource_count {0};
        int32_t original_resource_count {0};
        Range   timeline_range {0, 0};
        std::chrono::microseconds optimization_time;
    };

    struct ResourceOptimizerOptions {};

    class ResourceOptimizer
    {
    public:
        ResourceOptimizer(const Graph& graph, const ResourceOptimizerOptions& options)
        : m_nodes(graph.get_nodes_vector()), m_edges(graph.edges), m_options(options)
        {
        }

        ResourceOptimizerResult run()
        {
            const auto start_time = std::chrono::utc_clock::now();
            const auto R = evaluate_required_resources();

            std::vector<OptimizerResource> gen_resources;

            const auto end_time = std::chrono::utc_clock::now();
        }

    private:
        std::vector<resource_info> evaluate_required_resources() const
        {
            std::vector<resource_info> result;

            // Find output resources
            for (size_t i = 0; i < m_nodes.size(); i++)
            {
                node_ptr node = m_nodes[i];
                auto& claims = node->resource_claims();
                for (const auto& resource : claims)
                {
                    if (resource.usage() == ResourceUsage::eInput) continue;
                    result.push_back(resource_info::create_from(*node, resource, i));
                }
            }

            // Find resource Consumers
            for (auto& res_info : result)
            {
                for (const auto& edge : m_edges)
                {
                    if (res_info.origin_node_id != edge.start.node_id)     continue;
                    if (res_info.origin_node_id == edge.end.node_id)       continue;
                    if (res_info.origin_res_id  != edge.start.resource_id) continue;

                    // ATP: Non-origin Node, Edge starts from current resource
                    int32_t consumer_id     = edge.end.node_id;
                    int32_t consumer_res_id = edge.end.resource_id;

                    auto iter = std::ranges::find_if(m_nodes, [&](const node_ptr& node){
                        return node->id() == consumer_id;
                    });

                    auto consumer_node_idx = static_cast<int32_t>(std::distance(std::begin(m_nodes), iter));
                    node_ptr consumer_node = m_nodes[consumer_node_idx];

                    consumer_info consumer = {
                        .node_id = consumer_id,
                        .node_idx = consumer_node_idx,
                        .node_name = consumer_node->name(),
                        .resource_id = consumer_res_id,
                        .resource_name = edge.end.resource_name,
                        .usage = consumer_node->get_resource(consumer_res_id).usage(),
                        .node = consumer_node,
                    };

                    res_info.consumers.push_back(consumer);
                }
            }

            return result;
        }

        static std::set<usage_point> get_usage_points_for_resource_info(const resource_info& resource_info)
        {
            std::set<usage_point> usage_points;

            usage_point producer_usage_point(resource_info);
            usage_points.insert(producer_usage_point);

            for (auto& consumer : resource_info.consumers)
            {
                usage_point consumer_point(consumer);
                usage_points.insert(consumer_point);
            }

            return usage_points;
        }

        int32_t                         m_id_sequence {0};
        const ResourceOptimizerOptions& m_options;
        const std::vector<node_ptr>     m_nodes;
        const std::vector<Edge>&        m_edges;
    };
}