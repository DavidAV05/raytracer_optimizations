
#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "Hittable.h"

class HittableList final : public Hittable {
public:
    HittableList() = default;

    explicit HittableList(std::shared_ptr<Hittable> object) {
        add(std::move(object));
    }

    void clear() {
        objects_.clear();
    }

    void add(std::shared_ptr<Hittable> object) {
        objects_.push_back(std::move(object));
    }

    [[nodiscard]] bool hit(const Ray& ray, const Interval interval, HitRecord& record) const override {
        HitRecord tempRecord;
        bool hitAnything = false;
        double closestSoFar = interval.max();

        for (const auto& object : objects_) {
            const Interval searchInterval{interval.min(),closestSoFar};
            if (object->hit(ray, searchInterval, tempRecord)) {
                hitAnything = true;
                closestSoFar = tempRecord.t;
                record = tempRecord;
            }
        }

        return hitAnything;
    }

    [[nodiscard]] const std::vector<std::shared_ptr<Hittable>>& objects() const noexcept {
        return objects_;
    }

private:
    std::vector<std::shared_ptr<Hittable>> objects_;
};
