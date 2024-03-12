#pragma once

#include <type_traits>

namespace Nebula::nrg
{
    class Resource
    {
    public:
        template <typename T>
        T& as()
        {
            static_assert(std::is_base_of_v<Resource, T>, "Template parameter T must be a valid Resource type");
            return dynamic_cast<T&>(*this);
        }
    };
}