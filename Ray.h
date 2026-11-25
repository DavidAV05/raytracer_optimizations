
#pragma once

#include "Vec3.h"

class Ray {
public:
    constexpr Ray() = default;

    constexpr Ray(const Point3& origin, const Vec3& direction)
        : origin_(origin), direction_(direction) {}

    [[nodiscard]] constexpr const Point3& origin() const {
        return origin_;
    }

    [[nodiscard]] constexpr const Vec3& direction() const {
        return direction_;
    }

    [[nodiscard]] constexpr Point3 at(double t) const {
        return origin_ + (t * direction_);
    }

private:
    Point3 origin_{};
    Vec3 direction_{};
};
using Point3 = Vec3;