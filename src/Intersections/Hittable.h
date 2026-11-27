#pragma once

#include "Ray.h"
#include "../Types/Interval.h"
#include "Aabb.h"
#include "../../Materials/Material.h"

struct HitRecord {
    Point3 intersectionPoint;
    Vec3 normal;
    double t{};
    bool frontFace{};

    std::shared_ptr<const Material> material;

    void setFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        frontFace = ray.direction().dot(outwardNormal) < 0.0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;

    [[nodiscard]] virtual bool hit(const Ray& ray, Interval interval, HitRecord& outRec) const = 0;

    [[nodiscard]] virtual bool boundingBox(Aabb& output) const {
        (void)output;
        return false;
    }
};