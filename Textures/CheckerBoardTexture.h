#pragma once

#include <cassert>
#include <cmath>

#include "Texture.h"

class CheckerTexture final : public Texture {
public:
    CheckerTexture(
        const double squareSize,
        const Color& colorA,
        const Color& colorB
    )
        : squareSize_(squareSize),
          colorA_(colorA),
          colorB_(colorB) {
        assert(squareSize_ > 0.0);
    }

    [[nodiscard]] Color value(
        const Point3& point
    ) const override {

        const int cellX = static_cast<int>(std::floor(point.x() / squareSize_));
        const int cellZ = static_cast<int>(std::floor(point.z() / squareSize_));

        if ((cellX + cellZ) % 2 != 0) {
            return colorA_;
        }

        return colorB_;
    }

private:
    double squareSize_;
    Color colorA_;
    Color colorB_;
};