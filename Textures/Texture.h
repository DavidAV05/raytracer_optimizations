#pragma once

#include "../Color.h"
#include "../Vec3.h"

class Texture {
public:
    virtual ~Texture() = default;

    [[nodiscard]] virtual Color value(
        const Point3& point
    ) const = 0;
};