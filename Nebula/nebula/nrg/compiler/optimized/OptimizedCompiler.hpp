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
    public:
        explicit OptimizedCompiler(const std::shared_ptr<Context>& context);

        CompilerResult compile(const Graph& graph) override;

        ~OptimizedCompiler() override = default;
    };
}