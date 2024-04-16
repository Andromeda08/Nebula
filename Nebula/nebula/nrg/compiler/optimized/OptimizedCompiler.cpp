#include "OptimizedCompiler.hpp"

#include <format>
#include <sstream>
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
        logs.push_back(std::format("Compiling started at {:%Y-%m-%d %H:%M}", result.start_timestamp));
        logs.push_back(fmt_nodes_str(std::format("Input Nodes ({}):", nodes.size()), nodes));

        // 1. Cull unreachable nodes
        std::vector<node_ptr> connected_nodes;
        try {
            connected_nodes = filter_unreachable_nodes(nodes);
        }
        catch (const std::runtime_error& ex) {
            make_failed_result(result, ex.what());
            return result;
        }

        std::size_t culled_node_cnt = nodes.size() - connected_nodes.size();
        if (culled_node_cnt != 0)
        {
            logs.push_back(std::format("Found and culled {} unreachable node(s).", culled_node_cnt));
        }
        else
        {
            logs.emplace_back("No unreachable nodes were found.");
        }

        logs.push_back(fmt_nodes_str(std::format("Remaining Nodes ({}):", connected_nodes.size()), connected_nodes));

        // 2. Execution order
        std::vector<node_ptr> execution_order;
        try {
            execution_order = get_execution_order(connected_nodes);
        }
        catch (const std::runtime_error& ex) {
            make_failed_result(result, ex.what());
            return result;
        }

        logs.push_back(fmt_nodes_str(std::format("Execution order: ({}):", execution_order.size()), execution_order));

        // 3. Resource optimization
        ResourceOptimizerOptions optimizer_options {
            .export_result = true,
        };
        ResourceOptimizerResult optimizer_result;
        auto resource_optimizer = std::make_shared<ResourceOptimizer>(execution_order, edges, optimizer_options);\

        try {
            optimizer_result = resource_optimizer->run();
        }
        catch (const std::runtime_error& ex) {
            make_failed_result(result, ex.what());
            return result;
        }

        // 4. Create Resources

        return result;
    }

    std::string OptimizedCompiler::fmt_nodes_str(const std::string& prefix, const std::vector<node_ptr>& nodes)
    {
        std::stringstream input_nodes_str;
        input_nodes_str << prefix;
        for (const auto& node : nodes)
        {
            input_nodes_str << std::format(" [{}]", node->name());
        }
        return input_nodes_str.str();
    }

    void OptimizedCompiler::make_failed_result(CompilerResult& result, const std::string& error_message)
    {
        result.success = false;
        result.failure_message = error_message;
    }
}