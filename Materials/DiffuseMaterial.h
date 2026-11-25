#pragma once

#include <memory>
#include <utility>

#include "Material.h"
#include "../Textures/Texture.h"
class DiffuseMaterial final : public Material
{
public:
    explicit DiffuseMaterial(std::shared_ptr<const Texture> albedoTexture)
        : albedoTexture_(std::move(albedoTexture))
    {
    }

    [[nodiscard]] Color albedo(const Point3& point) const override
    {
        return albedoTexture_->value(point);
    }

private:
    std::shared_ptr<const Texture> albedoTexture_;
};
