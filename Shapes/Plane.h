
#pragma once

#include <cmath>
#include <memory>
#include <utility>

#include "../src/Intersections/Hittable.h"
#include "../src/Types/Interval.h"
#include "../Materials/Material.h"
#include "../src/Intersections/Ray.h"
#include "../src/Types/Vec3.h"


class Plane final : public Hittable {
public:
    Plane(
        const Point3& point,
        const Vec3& normal,
        std::shared_ptr<const Material> material
    )
        : point_(point),
          normal_(normal.normalized()),
          material_(std::move(material)) {}

    [[nodiscard]] bool hit(
        const Ray& ray,
        Interval rayInterval,
        HitRecord& record
    ) const override {

        double denominator = ray.direction().dot(normal_);

        if (std::abs(denominator) < 1e-6) return false;

        double t = ((point_ - ray.origin()).dot(normal_)) / denominator;

        if (!rayInterval.surrounds(t)) return false;

        record.t = t;
        record.intersectionPoint = ray.at(t);
        record.setFaceNormal(ray, normal_);
        record.material = material_;
        return true;
    }

private:
    Point3 point_;
    Vec3 normal_;
    std::shared_ptr<const Material> material_;
};
