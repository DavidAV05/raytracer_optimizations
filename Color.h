#pragma once

#include <cmath>
#include <ostream>

#include "Interval.h"
#include "Vec3.h"

using Color = Vec3;

[[nodiscard]] constexpr Color multiplyColors(
    const Color& a,
    const Color& b
) noexcept {
    return Color{
        a.x() * b.x(),
        a.y() * b.y(),
        a.z() * b.z()
    };
}

/**
 * @brief Converts a linear color component to gamma-2 encoded space.
 *
 * Negative values are mapped to zero.
 */
[[nodiscard]] inline double linearToGamma(double linearComponent) {
    if (linearComponent > 0.0) {
        return std::sqrt(linearComponent);
    }

    return 0.0;
}

/**
 * @brief Writes a linear RGB color as an 8-bit PPM pixel.
 *
 * Applies gamma-2 correction before clamping and quantization.
 *
 * @param out Output stream.
 * @param pixelColor Linear RGB color.
 */
inline void writeColor(std::ostream& out, const Color& pixelColor) {
    const double r = linearToGamma(pixelColor.x());
    const double g = linearToGamma(pixelColor.y());
    const double b = linearToGamma(pixelColor.z());

    constexpr Interval intensity{0.0, 0.999};

    const int red = static_cast<int>(256 * intensity.clamp(r));
    const int green = static_cast<int>(256 * intensity.clamp(g));
    const int blue = static_cast<int>(256 * intensity.clamp(b));

    out << red << ' ' << green << ' ' << blue << '\n';
}
