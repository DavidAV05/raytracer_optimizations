#pragma once

#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numbers>
#include <stdexcept>

#include "../Types//Color.h"
#include "../Intersections/Ray.h"
#include "../Intersections/RayTracer.h"
#include "../Scene/Scene.h"
#include "../Types/SimpleRT.h"
#include "../Types/Vec3.h"

class Camera final
{
public:
    double aspectRatio = 4.0 / 3.0;
    int imageWidth = 400;


    double verticalFov = 90.0;

    Point3 lookFrom{0.0, 0.0, 0.0};
    Point3 lookAt{0.0, 0.0, -1.0};


    void initialize()
    {
        if (imageWidth <= 0 ||
            !(aspectRatio > 0.0))
        {
            throw std::invalid_argument(
                "Camera width, aspect ratio, and samples per pixel, and depth must be positive"
            );
        }

        if (!(verticalFov > 0.0 && verticalFov < 180.0))
        {
            throw std::invalid_argument("Vertical FOV must be between 0 and 180 degrees");
        }

        imageHeight_ = std::max(
            1,
            static_cast<int>(imageWidth / aspectRatio)
        );

        center_ = lookFrom;

        const Vec3 forward = (lookAt - lookFrom).normalized();
        const Vec3 right = forward.cross(Vec3{0, 1, 0}).normalized();
        const Vec3 up = right.cross(forward);

        constexpr double focalLength = 1.0;
        const double fovRad = verticalFov * std::numbers::pi / 180.0;
        const double viewportHeight = 2 * focalLength * std::tan(fovRad / 2);

        const double viewportWidth = viewportHeight * static_cast<double>(imageWidth) / imageHeight_;

        const Vec3 viewportU = viewportWidth * right;
        const Vec3 viewportV = -viewportHeight * up;

        pixelDeltaU_ = viewportU / imageWidth;
        pixelDeltaV_ = viewportV / imageHeight_;

        const Point3 viewportUpperLeft =
            center_
            + focalLength * forward
            - viewportU / 2.0
            - viewportV / 2.0;

        pixel00Location_ = viewportUpperLeft + 0.5 * (pixelDeltaU_ + pixelDeltaV_);
    }

    [[nodiscard]] Ray getPrimaryRay(int x, int y, const bool jitter = true) const
    {
        const Vec3 offset = jitter ? sampleSquare() : Vec3{0.0, 0.0, 0.0};

        const Point3 pixelSample = pixel00Location_
            + (x + offset.x()) * pixelDeltaU_
            + (y + offset.y()) * pixelDeltaV_;

        const Point3 rayOrigin = center_;
        const Vec3 rayDirection = pixelSample - rayOrigin;

        return Ray{rayOrigin, rayDirection};
    }


    [[nodiscard]] int imageHeight() const noexcept
    {
        return imageHeight_;
    }

private:
    int imageHeight_{};
    Point3 center_{};
    Point3 pixel00Location_{};
    Vec3 pixelDeltaU_{};
    Vec3 pixelDeltaV_{};


    [[nodiscard]] static Vec3 sampleSquare()
    {
        return Vec3{
            simpleRT::randomDouble() - 0.5,
            simpleRT::randomDouble() - 0.5,
            0.0
        };
    }
};
