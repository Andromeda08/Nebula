#pragma once

#include <string>
#include <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    struct ResourceSpecification
    {
        std::string     name  {"Unknown Resource"};
        ResourceUsage   usage {ResourceUsage::eUnknown};
        ResourceType    type  {ResourceType::eUnknown};
    };
}