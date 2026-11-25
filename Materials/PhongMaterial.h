#pragma once

#include <cassert>

#include "Material.h"
#include "../Textures/Texture.h"
#include <memory>
#include <utility>

class PhongMaterial final : public Material
{
public:
    PhongMaterial(
        std::shared_ptr<const Texture> albedoTexture,
        const double specularStrength,
        const double shininess,
        const double reflectivity,
        const Color reflectionTint
    )
        : albedoTexture_(std::move(albedoTexture)),
          specularStrength_(specularStrength),
          shininess_(shininess),
          reflectivity_(reflectivity),
          reflectionTint_(reflectionTint)
    {
        assert(albedoTexture_);
        assert(specularStrength >= 0.0 && specularStrength <= 1.0);
        assert(shininess > 0.0);
        assert(reflectivity >= 0.0 && reflectivity <= 1.0);
    }

    [[nodiscard]] Color albedo(const Point3& point) const override
    {
        return albedoTexture_->value(point);
    }

    [[nodiscard]] double specularStrength() const noexcept override
    {
        return specularStrength_;
    }

    [[nodiscard]] double shininess() const noexcept override
    {
        return shininess_;
    }

    [[nodiscard]] double reflectivity() const noexcept override
    {
        return reflectivity_;
    }

    [[nodiscard]] Color reflectionTint() const noexcept override
    {
        return reflectionTint_;
    }

private:
    std::shared_ptr<const Texture> albedoTexture_;
    double specularStrength_;
    double shininess_;
    double reflectivity_ = 0.0;
    Color reflectionTint_;
};
