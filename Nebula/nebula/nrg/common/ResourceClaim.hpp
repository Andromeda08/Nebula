#pragma once

#include <string>
#include <uuid.h>
#include <nmath/Utility.hpp>
#include <nrg/common/ResourceSpecification.hpp>
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    struct ResourceClaim
    {
        int32_t         id    {nmath::rand()};
        uuids::uuid     uuid = uuids::uuid_system_generator{}();
        std::string     name  {"Unknown Resource"};
        ResourceUsage   usage {ResourceUsage::eUnknown};
        ResourceType    type  {ResourceType::eUnknown};

        bool            input_connected {false};

        ResourceClaim() = default;

        ResourceClaim(const ResourceSpecification& rs)
        : name(rs.name), usage(rs.usage), type(rs.type)
        {
        }
    };
}