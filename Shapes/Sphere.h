
#pragma once

#include <cassert>
#include <cmath>

#include "../Hittable.h"
#include "../Materials/Material.h"


class Sphere final : public Hittable {
public:
    Sphere(const Point3& center, const double radius,std::shared_ptr<const Material> material)
       : center_(center),
         radius_(radius),
         material_(std::move(material)) {
        assert(radius_ > 0.0);
        assert(material_ != nullptr);
    }
    [[nodiscard]] bool hit(const Ray& ray, const Interval interval, HitRecord& record) const override {
        const Vec3 oc = ray.origin() - center_;

        const double a = ray.direction().lengthSquared();
        const double h = ray.direction().dot(oc);
        const double c = oc.lengthSquared() - radius_ * radius_;

        const double discriminant = h * h - a * c;

        if (discriminant < 0.0) {
            return false;
        }



        const double sqrtDiscriminant = std::sqrt(discriminant);

        double tRoot = (-h - sqrtDiscriminant) / a;

        if (!interval.surrounds(tRoot)) {
            tRoot = (-h + sqrtDiscriminant) / a;
            if (!interval.surrounds(tRoot)) {
                return false;
            }
        }

        record.t = tRoot;
        record.intersectionPoint = ray.at(tRoot);

        const Vec3 outwardNormal =
            (record.intersectionPoint - center_) / radius_;

        record.setFaceNormal(ray, outwardNormal);
        record.material =material_;
        return true;
    }

    [[nodiscard]] bool boundingBox(Aabb& output) const override {
        const Vec3 radius{radius_, radius_, radius_};
        output = Aabb{center_ - radius, center_ + radius};
        return true;
    }

private:
    Point3 center_;
    double radius_;
    std::shared_ptr<const Material> material_;
};
