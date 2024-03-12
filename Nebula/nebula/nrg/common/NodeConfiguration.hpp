#pragma once

namespace Nebula::nrg
{
    struct NodeConfiguration
    {
        virtual void render() { /* no-op default */ }

        virtual bool validate() { return true; }

        virtual ~NodeConfiguration() = default;
    };
}