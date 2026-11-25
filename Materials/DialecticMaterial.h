#pragma once

#include <cassert>

#include "./Material.h"
#include "../Textures/Texture.h"

class DielectricMaterial final : public Material
{
public:
    DielectricMaterial(const double refractionIndex, const Color& albedo, const Color& absorption)
        : refractiveIndex_(refractionIndex),
          albedo_(albedo),
          absorption_(absorption)
    {
        if (!std::isfinite(refractiveIndex_) || refractiveIndex_ <= 0.0)
        {
            throw std::invalid_argument(
                "Refractive index must be positive and finite"
            );
        }

        if (!std::isfinite(absorption_.x()) || absorption_.x() < 0.0 ||
            !std::isfinite(absorption_.y()) || absorption_.y() < 0.0 ||
            !std::isfinite(absorption_.z()) || absorption_.z() < 0.0)
        {
            throw std::invalid_argument(
                "Absorption coefficients must be finite and nonnegative"
            );
        }
    }


    [[nodiscard]] Color albedo(const Point3&) const override
    {
        return albedo_;
    }

    [[nodiscard]] bool isTransmissive() const noexcept override
    {
        return true;
    }

    [[nodiscard]] double refractiveIndex() const noexcept override
    {
        return refractiveIndex_;
    }

    [[nodiscard]] Color absorption() const noexcept override
    {
        return absorption_;
    }

private:
    double refractiveIndex_;
    Color albedo_;
    Color absorption_;
};
