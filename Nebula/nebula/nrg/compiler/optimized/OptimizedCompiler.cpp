#include "OptimizedCompiler.hpp"

#include <fmt/format.h>
#include <sstream>
#include <iostream>
#include "ResourceOptimizer.hpp"

namespace Nebula::nrg
{
    OptimizedCompiler::OptimizedCompiler(const std::shared_ptr<Context>& context)
    : CompilerStrategy(context)
    {
    }

    CompilerResult OptimizedCompiler::compile(const Graph& graph)
    {
        auto  nodes = graph.get_nodes_vector();
        auto& edges = graph.edges;

        CompilerResult result = {};
        std::vector<std::string>& logs = result.internal_logs;

        result.start_timestamp = std::chrono::utc_clock::now();
        logs.push_back(fmt::format("Compiling started at {:%Y-%m-%d %H:%M}", result.start_timestamp));
        logs.push_back(fmt_nodes_str(fmt::format("Input Nodes ({}):", nodes.size()), nodes));

        // 1. Cull unreachable nodes --------------------------------
        std::vector<node_ptr> connected_nodes;
        try {
            connected_nodes = filter_unreachable_nodes(nodes);

            std::size_t culled_node_cnt = nodes.size() - connected_nodes.size();
            if (culled_node_cnt != 0)
            {
                logs.push_back(fmt::format("Found and culled {} unreachable node(s).", culled_node_cnt));
            }
            else
            {
                logs.emplace_back("No unreachable nodes were found.");
            }
            logs.push_back(fmt_nodes_str(fmt::format("Remaining Nodes ({}):", connected_nodes.size()), connected_nodes));
        }
        catch (const std::runtime_error& ex) {
            make_failed_result(result, ex.what());
            return result;
        }

        // 2. Execution order ---------------------------------------
        std::vector<node_ptr> execution_order;
        try {
            execution_order = get_execution_order(connected_nodes);
            logs.push_back(fmt_nodes_str(fmt::format("Execution order: ({}):", execution_order.size()), execution_order));
        }
        catch (const std::runtime_error& ex) {
            make_failed_result(result, ex.what());
            return result;
        }

        // 3. Resource optimization ---------------------------------
        ResourceOptimizerOptions optimizer_options { true };
        ResourceOptimizerResult optimizer_result;
        auto resource_optimizer = std::make_shared<ResourceOptimizer>(execution_order, edges, optimizer_options);

        try {
            optimizer_result = resource_optimizer->run();
        }
        catch (const std::runtime_error& ex) {
            make_failed_result(result, ex.what());
            return result;
        }

        // 4. Create Resources --------------------------------------
        std::map<std::string, std::shared_ptr<Resource>> resources;
        for (const auto& gen_res : optimizer_result.resources)
        {
            auto name = fmt::format("({:%Y-%m-%d %H:%M}) Resource {}", result.start_timestamp, gen_res.id);

            ResourceCreateInfo create_info = {
                .claim       = gen_res.original_info.claim,
                .format      = gen_res.format,
                .name        = name,
                .type        = gen_res.type,
                .usage_flags = gen_res.usage_flags,
            };

            auto resource = m_resource_factory->create(create_info);

            if (!resource)
            {
                logs.push_back(fmt::format("Failed to create {} resource: {}", to_string(gen_res.type), name));
                continue;
            }

            logs.push_back(fmt::format("Created {} resource: {}", to_string(gen_res.type), name));
            resources.insert({ std::to_string(gen_res.id), resource });
        }

        // 5. Create Nodes ------------------------------------------
        std::vector<std::shared_ptr<Node>> rg_nodes;
        std::map<int32_t, int32_t> node_mapping; // Graph ID -> RG ID
        for (const auto& node : execution_order)
        {
            auto rgn = m_node_factory->create(node);
            if (rgn != nullptr)
            {
                rg_nodes.push_back(rgn);
                node_mapping.insert({node->id(), static_cast<int32_t>(rg_nodes.size() - 1) });
            }
        }

        // 6. Connect resources to nodes ----------------------------
        for (const auto& opt_resource : optimizer_result.resources)
        {
            // 6.0 Get resource
            auto& resource = resources[std::to_string(opt_resource.id)];

            // 6.1 Connect to origin node
            auto& origin = opt_resource.original_info;
            auto& cnode_origin = rg_nodes[node_mapping[origin.origin_node_id]];
            cnode_origin->set_resource(origin.origin_res_name, resource);

            // 6.2 Connect to consumer nodes
            for (const auto& consumer : opt_resource.usage_points)
            {
                auto& cnode_consumer = rg_nodes[node_mapping[consumer.user_node_id]];
                cnode_consumer->set_resource(consumer.used_as, resource);
            }
        }

        // 7. Create RenderPath -------------------------------------
        auto render_path = std::make_shared<RenderPath>(std::move(rg_nodes), std::move(resources));

        // 8. Fill & Finalize result --------------------------------
        result.render_path = render_path;
        result.success = true;

        return result;
    }

    std::string OptimizedCompiler::fmt_nodes_str(const std::string& prefix, const std::vector<node_ptr>& nodes)
    {
        std::stringstream input_nodes_str;
        input_nodes_str << prefix;
        for (const auto& node : nodes)
        {
            input_nodes_str << fmt::format(" [{}]", node->name());
        }
        return input_nodes_str.str();
    }

    void OptimizedCompiler::make_failed_result(CompilerResult& result, const std::string& error_message)
    {
        result.success = false;
        result.failure_message = error_message;
    }
}