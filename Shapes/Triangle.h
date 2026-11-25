#pragma once

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <utility>

#include "../Hittable.h"
#include "../Materials/Material.h"

class Triangle final : public Hittable {
public:
    Triangle(
        const Point3& a,
        const Point3& b,
        const Point3& c,
        std::shared_ptr<const Material> material
    )
        : a_(a),
          edge1_(b - a),
          edge2_(c - a),
          normal_(edge1_.cross(edge2_)),
          material_(std::move(material)) {
        if (normal_.lengthSquared() == 0.0) {
            throw std::invalid_argument("Triangle vertices must not be collinear");
        }
        if (!material_) {
            throw std::invalid_argument("Triangle material must not be null");
        }
        normal_ = normal_.normalized();
    }

    [[nodiscard]] bool hit(const Ray& ray,const Interval rayInterval,HitRecord& record) const override
    {
        const Vec3 p = ray.direction().cross(edge2_);
        const double determinant = edge1_.dot(p);

        if (std::abs(determinant) < 1e-8) {
            return false;
        }

        const double inverseDeterminant = 1.0 / determinant;
        const Vec3 fromA = ray.origin() - a_;
        const double u = fromA.dot(p) * inverseDeterminant;

        if (u < 0.0 || u > 1.0) {
            return false;
        }

        const Vec3 q = fromA.cross(edge1_);
        const double v = ray.direction().dot(q) * inverseDeterminant;

        if (v < 0.0 || u + v > 1.0) {
            return false;
        }

        const double t = edge2_.dot(q) * inverseDeterminant;

        if (!rayInterval.surrounds(t)) {
            return false;
        }

        record.t = t;
        record.intersectionPoint = ray.at(t);
        record.setFaceNormal(ray, normal_);
        record.material = material_;
        return true;
    }

    [[nodiscard]] bool boundingBox(Aabb& output) const override {
        constexpr double padding = 1e-8;
        const Point3 b = a_ + edge1_;
        const Point3 c = a_ + edge2_;
        output = Aabb{
            Point3{
                std::min({a_.x(), b.x(), c.x()}) - padding,
                std::min({a_.y(), b.y(), c.y()}) - padding,
                std::min({a_.z(), b.z(), c.z()}) - padding
            },
            Point3{
                std::max({a_.x(), b.x(), c.x()}) + padding,
                std::max({a_.y(), b.y(), c.y()}) + padding,
                std::max({a_.z(), b.z(), c.z()}) + padding
            }
        };
        return true;
    }

private:
    Point3 a_;
    Vec3 edge1_;
    Vec3 edge2_;
    Vec3 normal_;
    std::shared_ptr<const Material> material_;
};
