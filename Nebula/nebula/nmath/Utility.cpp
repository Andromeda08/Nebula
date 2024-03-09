#include "Utility.hpp"

namespace Nebula::nmath
{
    std::default_random_engine Random::s_generator = std::default_random_engine();
    std::uniform_real_distribution<float> Random::s_float_distribution = std::uniform_real_distribution<float>(0.0, 1.0);
    std::uniform_int_distribution<int> Random::s_integer = std::uniform_int_distribution<int>();
}