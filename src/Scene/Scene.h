
#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include "../Types/Color.h"
#include "../Intersections/Hittable.h"
#include "../Intersections/HittableList.h"
#include "../Types/Interval.h"
#include "../../lights/Pointlight.h"
#include "../Intersections/Ray.h"
#include "../../Shapes/Triangle.h"

class Scene final {
public:
    void addObject(std::shared_ptr<Hittable> object) {
        ++objectCount_;
        if (std::dynamic_pointer_cast<Triangle>(object)) {
            ++triangleCount_;
        }
        objects_.add(std::move(object));
    }

    [[nodiscard]] std::size_t objectCount() const noexcept {
        return objectCount_;
    }

    [[nodiscard]] std::size_t triangleCount() const noexcept {
        return triangleCount_;
    }

    void addLight(const PointLight& light) {
        lights_.push_back(light);
    }

    [[nodiscard]] bool hit(
        const Ray& ray,
        Interval rayInterval,
        HitRecord& record
    ) const {
        return objects_.hit(ray, rayInterval, record);
    }

    [[nodiscard]] const std::vector<PointLight>& lights() const noexcept {
        return lights_;
    }

    [[nodiscard]] const Color& ambientLight() const noexcept {
        return ambientLight_;
    }

    void setAmbientLight(const Color& color) noexcept {
        ambientLight_ = color;
    }

private:
    HittableList objects_;
    std::size_t objectCount_{};
    std::size_t triangleCount_{};
    std::vector<PointLight> lights_;
    Color ambientLight_{0.1, 0.1, 0.1};
};
