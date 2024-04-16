#pragma once

#include <memory>
#include <nrg/common/Context.hpp>
#include <nrg/compiler/CompilerResult.hpp>
#include <nrg/compiler/CompilerStrategy.hpp>
#include <nrg/editor/Graph.hpp>

namespace Nebula::nrg
{
    class OptimizedCompiler : public CompilerStrategy
    {
        using node_ptr = std::shared_ptr<EditorNode>;

    public:
        explicit OptimizedCompiler(const std::shared_ptr<Context>& context);

        CompilerResult compile(const Graph& graph) override;

        ~OptimizedCompiler() override = default;

    private:
        static void make_failed_result(CompilerResult& result, const std::string& error_message);

        static std::string fmt_nodes_str(const std::string& prefix, const std::vector<node_ptr>& nodes);
    };
}