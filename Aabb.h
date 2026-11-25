#pragma once

#include <algorithm>
#include <limits>

#include "Interval.h"
#include "Ray.h"

class Aabb final {
public:
    Aabb() = default;

    Aabb(const Point3& minimum, const Point3& maximum)
        : minimum_(minimum), maximum_(maximum) {}

    [[nodiscard]] const Point3& minimum() const noexcept {
        return minimum_;
    }

    [[nodiscard]] const Point3& maximum() const noexcept {
        return maximum_;
    }

    [[nodiscard]] bool hit(const Ray& ray, Interval interval) const noexcept {
        for (int axis = 0; axis < 3; ++axis) {
            const double origin = component(ray.origin(), axis);
            const double direction = component(ray.direction(), axis);
            const double minimum = component(minimum_, axis);
            const double maximum = component(maximum_, axis);

            if (direction == 0.0) {
                if (origin < minimum || origin > maximum) {
                    return false;
                }
                continue;
            }

            double nearT = (minimum - origin) / direction;
            double farT = (maximum - origin) / direction;
            if (nearT > farT) {
                std::swap(nearT, farT);
            }

            interval = Interval{
                std::max(interval.min(), nearT),
                std::min(interval.max(), farT)
            };
            if (interval.max() < interval.min()) {
                return false;
            }
        }
        return true;
    }

    void include(const Aabb& other) noexcept {
        minimum_ = Point3{
            std::min(minimum_.x(), other.minimum_.x()),
            std::min(minimum_.y(), other.minimum_.y()),
            std::min(minimum_.z(), other.minimum_.z())
        };
        maximum_ = Point3{
            std::max(maximum_.x(), other.maximum_.x()),
            std::max(maximum_.y(), other.maximum_.y()),
            std::max(maximum_.z(), other.maximum_.z())
        };
    }

private:
    [[nodiscard]] static double component(const Vec3& value, int axis) noexcept {
        if (axis == 0) return value.x();
        if (axis == 1) return value.y();
        return value.z();
    }

    Point3 minimum_{
        std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity()
    };
    Point3 maximum_{
        -std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity()
    };
};
