#pragma once

#include <memory>
#include <nrg/common/Context.hpp>
#include <nrg/common/NodeType.hpp>
#include <nrg/editor/EditorNode.hpp>

namespace Nebula::nrg
{
    class EditorNodeFactory
    {
    public:
        explicit EditorNodeFactory(const std::shared_ptr<Context>& context);

        std::shared_ptr<EditorNode> create(NodeType node_type);

    private:
        std::shared_ptr<Context> m_context;
    };
}