#pragma once
#include <cmath>

class Vec3
{
public:
    constexpr Vec3() = default;

    constexpr Vec3(double x, double y, double z) : x_(x), y_(y), z_(z)
    {
    }

    [[nodiscard]] constexpr double x() const { return x_; }
    [[nodiscard]] constexpr double y() const { return y_; }
    [[nodiscard]] constexpr double z() const { return z_; }

    [[nodiscard]] constexpr Vec3 operator+(const Vec3& other) const
    {
        return {
            x_ + other.x_,
            y_ + other.y_,
            z_ + other.z_
        };
    }

    [[nodiscard]] constexpr Vec3 operator-(const Vec3& other) const
    {
        return {
            x_ - other.x_,
            y_ - other.y_,
            z_ - other.z_
        };
    }

    [[nodiscard]] constexpr Vec3 operator-() const
    {
        return {-x_, -y_, -z_};
    }

    [[nodiscard]] constexpr Vec3 operator*(double scalar) const
    {
        return {
            x_ * scalar,
            y_ * scalar,
            z_ * scalar
        };
    }

    [[nodiscard]] constexpr Vec3 operator/(double scalar) const
    {
        return {
            x_ / scalar,
            y_ / scalar,
            z_ / scalar
        };
    }

    [[nodiscard]] constexpr double dot(const Vec3& other) const
    {
        return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
    }

    [[nodiscard]] constexpr Vec3 cross(const Vec3& other) const
    {
        return {
            y_ * other.z_ - z_ * other.y_,
            z_ * other.x_ - x_ * other.z_,
            x_ * other.y_ - y_ * other.x_
        };
    }

    [[nodiscard]] constexpr double lengthSquared() const
    {
        return x_ * x_ + y_ * y_ + z_ * z_;
    }

    [[nodiscard]] double length() const
    {
        return std::sqrt(lengthSquared());
    }

    [[nodiscard]] Vec3 normalized() const
    {
        return *this / length();
    }

private:
    double x_ = 0.0;
    double y_ = 0.0;
    double z_ = 0.0;
};

using Point3 = Vec3;
using Color = Vec3;

[[nodiscard]] constexpr Vec3 operator*(double scalar, const Vec3& v)
{
    return v * scalar;
}


/**
 * reflects incoming direction around a surfacenormal
 * !! assumes the normal is normalized
 */
[[nodiscard]] constexpr Vec3 reflect(const Vec3& incoming, const Vec3& normal)
{
    const auto d = incoming.dot(normal) * normal;
    return incoming - 2.0 * d;
}

/**
 * computes refracted direction using Snell's law
 *
 * asssumes both vectors are normalized and the normal opposes
 * the incoming direction  >>  eta = n1 / n2
 *
 * only valid when refraction is possible (no total internal reflection)
 */
[[nodiscard]] inline Vec3 refract(const Vec3& incoming, const Vec3& normal, double eta)
{
    const double cosTheta1 = std::min((-incoming).dot(normal), 1.0);
    const double sinTheta1 = std::sqrt(1.0 - cosTheta1 * cosTheta1);
    if (eta * sinTheta1 > 1.0)
    {
        return reflect(incoming, normal);
    }

    const Vec3 rPerp = (eta * (incoming + cosTheta1 * normal));
    const double rPerpSquared = rPerp.lengthSquared();
    const Vec3 rParallel = -std::sqrt(std::max(0.0, 1.0 - rPerpSquared)) * normal;

    return rPerp + rParallel;
}

[[nodiscard]] inline double schlickReflectance(double cosine, double refractiveIndex)
{
    const double r0Base = (1.0 - refractiveIndex) / (1.0 + refractiveIndex);

    const double r0 = r0Base * r0Base;

    return r0 + (1.0 - r0) * std::pow(1.0 - cosine, 5);
}
