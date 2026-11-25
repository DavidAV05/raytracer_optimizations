#pragma once

#include "Texture.h"

class SolidColor final : public Texture {
public:
    explicit constexpr SolidColor(const Color& color) noexcept
        : color_(color) {}

    [[nodiscard]] Color value(
        const Point3& point
    ) const override {
        return color_;
    }

private:
    Color color_;
};