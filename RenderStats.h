#pragma once

#include <cstddef>
#include <iomanip>
#include <ostream>
#include <stdexcept>

struct RenderStats final
{
    double totalRenderingTimeSeconds{};
    std::size_t objectCount{};
    std::size_t triangleCount{};

    void writeJson(std::ostream& output) const
    {
        output << "{\n  \"total_rendering_time_seconds\": "
            << std::setprecision(9) << totalRenderingTimeSeconds
            << ",\n  \"object_count\": " << objectCount
            << ",\n  \"triangle_count\": " << triangleCount
            << "\n}\n";

        if (!output)
        {
            throw std::runtime_error("Failed while writing render_stats.json");
        }
    }
};
