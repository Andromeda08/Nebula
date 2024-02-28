#pragma once

#include <random>

namespace Nebula::nmath
{
    class Random
    {
    public:
        static std::default_random_engine               s_generator;
        static std::uniform_real_distribution<float>    s_float_distribution;
    };

    // Generate a random float in [0; 1]
    inline static float randf()
    {
        return Random::s_float_distribution(Random::s_generator);
    }

    // Generate a random float in [low; high]
    inline static float randf(float low, float high)
    {
        std::uniform_real_distribution<float> dist = (low > high)
                                                     ? std::uniform_real_distribution(high, low)
                                                     : std::uniform_real_distribution(low, high);

        return dist(Random::s_generator);
    }
}