#pragma once

#include <string>
#include <uuid.h>
#include <nmath/Utility.hpp>
#include <nrg/resource/Requirement.hpp>
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    struct ResourceClaim
    {
        int32_t         id    {nmath::rand()};
        uuids::uuid     uuid = uuids::uuid_system_generator{}();
        bool            input_connected {false};
        std::shared_ptr<Requirement> req;

        ResourceClaim() = default;

        explicit ResourceClaim(const std::shared_ptr<Requirement>& requirement): req(requirement) {}

        std::string   name()  const { return req ? req->name  : "Unknown Resource"; }
        ResourceType  type()  const { return req ? req->type  : ResourceType::eUnknown; }
        ResourceUsage usage() const { return req ? req->usage : ResourceUsage::eUnknown; }
    };
}