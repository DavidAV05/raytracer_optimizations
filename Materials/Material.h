#pragma once

#include "../Color.h"

class Material {
public:
    virtual ~Material() = default;

    [[nodiscard]] virtual Color albedo(const Point3& point) const = 0;

    [[nodiscard]] virtual double specularStrength() const noexcept {
        return 0.0;
    }
    [[nodiscard]] virtual double reflectivity() const {
        return 0.0;
    }
    [[nodiscard]] virtual double shininess() const noexcept {
        return 32.0;
    }

    [[nodiscard]] virtual Color reflectionTint() const
    {
        return Color{1.0, 1.0, 1.0};
    }

    [[nodiscard]] virtual bool isTransmissive() const noexcept
    {
        return false;
    }

    [[nodiscard]] virtual double refractiveIndex() const noexcept
    {
        return 1.0;
    }

    [[nodiscard]] virtual Color absorption() const noexcept
    {
        return Color{0.0, 0.0, 0.0};
    }
};