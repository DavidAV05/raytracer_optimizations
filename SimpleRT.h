#pragma once

#include <limits>
#include <numbers>
#include <random>

namespace simpleRT {

    inline constexpr double infinity =
        std::numeric_limits<double>::infinity();

    [[nodiscard]] constexpr double degreesToRadians(const double degrees) noexcept {
        return degrees * std::numbers::pi / 180.0;
    }

    [[nodiscard]] inline double randomDouble() {
        thread_local std::mt19937 generator{std::random_device{}()};
        thread_local std::uniform_real_distribution<double> distribution{0.0, 1.0};

        return distribution(generator);
    }

    [[nodiscard]] inline double randomDouble(double min, double max) {
        return min + (max - min) * randomDouble();
    }

} // namespace simpleRT
