#pragma once
#include "../Types/Color.h"
#include "Hittable.h"
#include "../Types/Interval.h"
#include "Ray.h"
#include "../Scene/Scene.h"
#include "../Types/SimpleRT.h"
#include "../../lights/Pointlight.h"
#include <functional>
#include <concepts>

template<typename AccelerationStructure>
class RayTracer final {
public:
    [[nodiscard]] Color trace(
        const Ray &ray,
        const Scene &scene,
        const AccelerationStructure &accelerationStructure,
        int depth,
        const bool shadowsEnabled = true
    ) const {
        HitRecord record;

        if (accelerationStructure.hit(ray, Interval{0.001, simpleRT::infinity}, record))
        // if (scene.hit(ray, Interval{0.001, simpleRT::infinity}, record))
        {
            // dielection / refraction
            if (record.material->isTransmissive()) {
                if (depth == 0) {
                    return Color{0.0, 0.0, 0.0};
                }

                const double refractiveIndex = record.material->refractiveIndex();
                const Vec3 unitDirection = ray.direction().normalized();

                constexpr double rayBias = 0.001;


                const Color attenuation = !record.frontFace
                                              ? beerLambert(record.material->absorption(),
                                                            record.t * ray.direction().length())
                                              : Color{1.0, 1.0, 1.0};

                if (std::abs(refractiveIndex - 1.0) < 1e-6) {
                    const Ray nextRay{record.intersectionPoint + rayBias * unitDirection, unitDirection};

                    const Color transmittedColor = trace(nextRay, scene, accelerationStructure, depth - 1,
                                                         shadowsEnabled);

                    return multiplyColors(transmittedColor, attenuation);
                }

                const double eta = record.frontFace ? 1.0 / refractiveIndex : refractiveIndex;

                const double cosTheta = std::min(
                    (-unitDirection).dot(record.normal),
                    1.0
                );

                const double sinTheta = std::sqrt(std::max(0.0, 1.0 - cosTheta * cosTheta));

                const bool cannotRefract = eta * sinTheta > 1.0;

                const double reflectance = cannotRefract ? 1.0 : schlickReflectance(cosTheta, refractiveIndex);

                Color result{0.0, 0.0, 0.0};

                if (reflectance > 0.0) {
                    const Vec3 reflectedDirection = reflect(unitDirection, record.normal);

                    const Ray reflectedRay{record.intersectionPoint + rayBias * record.normal, reflectedDirection};

                    result = result + reflectance * trace(reflectedRay, scene, accelerationStructure, depth - 1,
                                                          shadowsEnabled);
                }

                if (!cannotRefract && reflectance < 1.0) {
                    const Vec3 refractedDirection =
                            refract(unitDirection, record.normal, eta);

                    const Ray refractedRay{
                        record.intersectionPoint - rayBias * record.normal,
                        refractedDirection
                    };

                    result = result + (1.0 - reflectance)
                             * trace(refractedRay, scene, accelerationStructure, depth - 1, shadowsEnabled);
                }

                return multiplyColors(result, attenuation);
            }


            // ambient lighting
            const Color surfaceColor = record.material->albedo(record.intersectionPoint);
            Color result = multiplyColors(surfaceColor, scene.ambientLight());
            const Vec3 viewDirection = (-ray.direction()).normalized();


            // direct lighting
            for (const PointLight &light: scene.lights()) {
                const Vec3 toLight =
                        light.position - record.intersectionPoint;

                const double lightDistance = toLight.length();

                if (lightDistance <= 1e-12) {
                    continue;
                }

                const Vec3 lightDirection = toLight / lightDistance;

                const double diffuseIntensity = std::max(0.0, record.normal.dot(lightDirection));

                if (diffuseIntensity <= 0.0) {
                    continue;
                }

                Color transmission{1.0, 1.0, 1.0};

                // shadows
                if (shadowsEnabled) {
                    constexpr double shadowBias = 0.001;
                    const Point3 shadowOrigin =
                            record.intersectionPoint + shadowBias * record.normal;
                    const Ray shadowRay{shadowOrigin, lightDirection};
                    transmission = shadowTransmittance(shadowRay, lightDistance, accelerationStructure);
                }

                // diffuse
                const Color diffuse = light.intensity * diffuseIntensity * multiplyColors(surfaceColor, light.color);
                result = result + multiplyColors(diffuse, transmission);

                // blinn-phong specular
                if (record.material->specularStrength() <= 0.0) {
                    continue;
                }

                const Vec3 halfway = lightDirection + viewDirection;

                if (halfway.lengthSquared() <= 1e-12) {
                    continue;
                }

                const Vec3 H = halfway.normalized();

                const double specularFactor = std::pow(
                    std::max(0.0, record.normal.dot(H)),
                    record.material->shininess()
                );

                const Color specular =
                        (record.material->specularStrength()
                         * light.intensity
                         * specularFactor)
                        * light.color;


                result = result + multiplyColors(specular, transmission);
            }


            // reflection
            if (depth > 0 && record.material->reflectivity() > 0.0) {
                const Vec3 reflectedDirection = reflect(ray.direction().normalized(), record.normal);
                const auto reflectedRay = Ray{record.intersectionPoint + 0.001 * record.normal, reflectedDirection};
                const double r = record.material->reflectivity();
                const Color reflectedColor =
                        trace(reflectedRay, scene, accelerationStructure, depth - 1, shadowsEnabled);
                const Color tint = record.material->reflectionTint();
                result = (1.0 - r) * result + r * multiplyColors(reflectedColor, tint);
            }


            return result;
        }

        // background (sky)
        const Vec3 unitDirection = ray.direction().normalized();
        const double t = 0.5 * (unitDirection.y() + 1.0);
        constexpr Color white{1.0, 1.0, 1.0};
        constexpr Color blue{0.5, 0.7, 1.0};

        return (1.0 - t) * white + t * blue;
    }

private:
    [[nodiscard]] static Color shadowTransmittance(const Ray &shadowRay, double lightDistance,
                                                   const AccelerationStructure& accelerationStructure) {
        Color transmission{1.0, 1.0, 1.0};

        double tMin = 0.001;
        double entryT = 0.0;

        Color currentAbsorption{0.0, 0.0, 0.0};
        bool insideGlass = false;

        HitRecord hitRecord;

        while (accelerationStructure.hit(shadowRay, Interval{tMin, lightDistance}, hitRecord)) {
            // opaque surfaces block all direct light
            if (!hitRecord.material->isTransmissive()) {
                return Color{0.0, 0.0, 0.0};
            }

            if (hitRecord.frontFace) {
                entryT = hitRecord.t;
                currentAbsorption = hitRecord.material->absorption();
                insideGlass = true;
            } else {
                const double distanceInside = hitRecord.t - entryT;

                const Color attenuation = beerLambert(currentAbsorption, distanceInside);

                transmission = multiplyColors(transmission, attenuation);

                insideGlass = false;
            }

            tMin = hitRecord.t + 0.001;
        }

        // ff the light is inside glass, account for the remaining distance
        if (insideGlass) {
            const double distanceInside = lightDistance - entryT;

            const Color attenuation = beerLambert(currentAbsorption, distanceInside);

            transmission = multiplyColors(transmission, attenuation);
        }

        return transmission;
    }

    [[nodiscard]] static Color beerLambert(const Color &absorption, double distance) {
        return
        {
            std::exp(-absorption.x() * distance),
            std::exp(-absorption.y() * distance),
            std::exp(-absorption.z() * distance)
        };
    }
};
