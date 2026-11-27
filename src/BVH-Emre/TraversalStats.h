#pragma once

#include <cstdint>

struct TraversalStats
{
    std::uint64_t traversalSteps = 0;
    std::uint64_t boundsTests = 0;
    std::uint64_t intersectionTests = 0;

    void reset() noexcept
    {
        traversalSteps = 0;
        boundsTests = 0;
        intersectionTests = 0;
    }
};