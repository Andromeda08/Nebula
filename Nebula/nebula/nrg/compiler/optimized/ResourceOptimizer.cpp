#include "ResourceOptimizer.hpp"

#include <bitset>
#include <format>
#include <fstream>
#include <queue>

namespace Nebula::nrg
{
    ResourceOptimizer::ResourceOptimizer(const std::vector<node_ptr>& nodes, const std::vector<Edge>& edges,
                                         const ResourceOptimizerOptions& options)
    : m_nodes(nodes), m_edges(edges), m_options(options)
    {
    }

    ResourceOptimizerResult ResourceOptimizer::run()
    {
        std::vector<std::string> logs;

        const auto start_time = std::chrono::utc_clock::now();
        const auto R = evaluate_required_resources();

        std::vector<OptimizerResource> gen_resources;

        uint32_t non_optimizable_count{0};
        for (const auto& r: R) {
            OptimizerResource resource = {
                .id = m_id_sequence++,
                .usage_points = {},
                .original_info = r,
                .type = r.type,
            };

            if (resource.type == ResourceType::eImage) {
                resource.format = r.claim.req->as<ImageRequirement>().format;
                resource.usage_flags = r.claim.req->as<ImageRequirement>().usage_flags;
            }

            auto& usage_points = resource.usage_points;
            usage_points = get_usage_points_for_resource_info(r);

            Range incoming_range(usage_points);

            // Case: Non-Optimizable Resource Type
            if (!r.optimizable) {
                gen_resources.push_back(resource);
                non_optimizable_count++;
                logs.push_back(std::format("New, non-optimizable resource with id {} of type {}",
                                           resource.id, to_string(resource.type)));
                continue;
            }

            // Case: No generated resources yet
            if (gen_resources.empty()) {
                gen_resources.push_back(resource);
                logs.push_back(std::format("New resource with id {} of type {}",
                                           resource.id, to_string(resource.type)));
                continue;
            }

            // Case: There are generated resources, try inserting into an existing one
            bool was_inserted = false;
            for (auto& timeline: gen_resources) {
                Range current_range = timeline.get_usage_range();
                std::bitset<5> flags;
                {
                    flags[0] = !current_range.overlaps(incoming_range);
                    flags[1] = r.optimizable;
                    flags[2] = r.type == timeline.type;
                    flags[3] = true; // Image Format compatibility
                }

                if (flags[2] && r.type == ResourceType::eImage) {
                    auto tl_req = timeline.original_info.claim.req->as<ImageRequirement>();
                    auto in_req = r.claim.req->as<ImageRequirement>();
                    flags[3] = tl_req.format == in_req.format;

                    // Combine image usage flags
                    tl_req.usage_flags |= in_req.usage_flags;
                }

                if (flags.all()) {
                    was_inserted = timeline.insert_usage_points(usage_points);
                    if (was_inserted) {
                        gen_resources.push_back(resource);
                        logs.push_back(
                            std::format(
                                "Resource with id {} of type {} was reused in range [{}, {}], {} new usage points were added",
                                timeline.id, to_string(timeline.type),
                                std::min_element(std::begin(usage_points), std::end(usage_points))->point,
                                std::max_element(std::begin(usage_points), std::end(usage_points))->point,
                                usage_points.size())
                        );
                    }
                    break;
                }
            }

            // Case: Failed to Insert
            if (!was_inserted) {
                gen_resources.push_back(resource);
                logs.push_back(std::format("New resource with id {} of type {}",
                                           resource.id, to_string(resource.type)));
            }
        }

        const auto end_time = std::chrono::utc_clock::now();

        ResourceOptimizerResult result = {
            .resources = gen_resources,
            .original_resources = R,
            .non_optimizable_count = non_optimizable_count,
            .optimized_resource_count = static_cast<uint32_t>(gen_resources.size()),
            .original_resource_count = static_cast<uint32_t>(R.size()),
            .timeline_range = {0, static_cast<int32_t>(m_nodes.size() - 1)},
            .optimization_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time),
            .start_time = start_time,
            .logs = logs,
        };

        if (m_options.export_result) {
            export_result(result);
        }

        return result;
    }

    void ResourceOptimizer::export_result(const ResourceOptimizerResult& result)
    {
        std::string file_name = std::format("resource_optimizer_{:%Y-%m-%d_%H-%M}.csv", result.start_time);

        std::vector<std::string> csv;
        std::stringstream sstr;
        sstr << "Optimized Resources,\n"
             << std::format("Reduction: {},", result.original_resource_count - result.optimized_resource_count)
             << std::format("Non-optimizable: {},", result.non_optimizable_count)
             << std::format("Time: {} microseconds", result.optimization_time.count());
        csv.push_back(sstr.str());
        sstr.str(std::string());

        sstr << ",";
        for (int32_t i = 0; i <= result.timeline_range.end; i++)
        {
            sstr << std::format("Node #{},", i);
        }
        csv.push_back(sstr.str());
        sstr.str(std::string());

        for (int32_t i = 0; i < result.resources.size(); i++) {
            auto& resource = result.resources[i];
            auto range = resource.get_usage_range();

            sstr << std::format("Resource #{},", i);
            for (int32_t j = 0; j <= result.timeline_range.end; j++) {
                auto usage_point = resource.get_usage_point(j);
                if (usage_point.has_value()) {
                    auto point = usage_point.value();
                    sstr << ((range.start == range.end) ? std::format("[{}]", point.used_as)
                        : (j == range.start) ? std::format("[{}", point.used_as)
                        : (j == range.end) ? std::format("{}]", point.used_as)
                        : point.used_as);
                }
                sstr << ",";
            }
            csv.push_back(sstr.str());
            sstr.str(std::string());
        }

        std::fstream fs(file_name);
        fs.open(file_name, std::ios_base::out);
        std::ostream_iterator<std::string> os_it(fs, "\n");
        std::copy(std::begin(csv), std::end(csv), os_it);
        fs.close();
    }

    std::vector<resource_info> ResourceOptimizer::evaluate_required_resources() const
    {
        std::vector<resource_info> result;

        // Find output resources
        for (size_t i = 0; i < m_nodes.size(); i++) {
            node_ptr node = m_nodes[i];
            auto& claims = node->resource_claims();
            for (const auto& resource: claims) {
                if (resource.usage() == ResourceUsage::eInput) continue;
                result.push_back(resource_info::create_from(*node, resource, i));
            }
        }

        // Find resource Consumers
        for (auto& res_info: result) {
            for (const auto& edge: m_edges) {
                if (res_info.origin_node_id != edge.start.node_id) continue;
                if (res_info.origin_node_id == edge.end.node_id) continue;
                if (res_info.origin_res_id != edge.start.resource_id) continue;

                // ATP: Non-origin Node, Edge starts from current resource
                int32_t consumer_id = edge.end.node_id;
                int32_t consumer_res_id = edge.end.resource_id;

                auto iter = std::ranges::find_if(m_nodes, [&](const node_ptr& node)
                {
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

    std::set<usage_point> ResourceOptimizer::get_usage_points_for_resource_info(const resource_info& resource_info)
    {
        std::set<usage_point> usage_points;

        usage_point producer_usage_point(resource_info);
        usage_points.insert(producer_usage_point);

        for (auto& consumer: resource_info.consumers) {
            usage_point consumer_point(consumer);
            usage_points.insert(consumer_point);
        }

        return usage_points;
    }
}