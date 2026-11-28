#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "BVH.h"
#include "TraversalStats.h"

#include "../Intersections/Ray.h"
#include "../Intersections/Hittable.h"
#include "../Types/Interval.h"

#include "../../Shapes/Triangle.h"
#include "../../Materials/PhongMaterial.h"
#include "../../Textures/SolidColor.h"


// Creates a material for BVH validation tests.
inline std::shared_ptr<const Material> createBVHTestMaterial()
{
    const auto texture = std::make_shared<SolidColor>(
        Color{0.9, 0.45, 0.16}
    );

    return std::make_shared<PhongMaterial>(
        texture,
        0.0,
        32.0,
        0.0,
        Color{1.0, 1.0, 1.0}
    );
}


// Brute Force implementation for validation.
inline bool bruteForceHit(
    const std::vector<std::shared_ptr<Hittable>>& objects,
    const Ray& ray,
    Interval interval,
    HitRecord& record,
    TraversalStats* stats = nullptr
) {
    bool hitAnything = false;
    double closestSoFar = interval.max();

    HitRecord tempRecord;

    for (const auto& object : objects) {

        // Count each triangle intersection attempt.
        if (stats) {
            ++stats->intersectionTests;
        }

        const Interval searchInterval{
            interval.min(),
            closestSoFar
        };

        if (object->hit(ray, searchInterval, tempRecord)) {

            hitAnything = true;
            closestSoFar = tempRecord.t;
            record = tempRecord;
        }
    }

    return hitAnything;
}


// Compares BVH results with Brute Force results.
inline void validateBVH(
    const std::vector<std::shared_ptr<Hittable>>& objects,
    const std::vector<Ray>& rays,
    Interval rayInterval,
    std::size_t maxLeafSize = 4
) {
    BVH bvh(objects, maxLeafSize);

    std::size_t hitMismatch = 0;
    std::size_t tMismatch = 0;
    std::size_t pointMismatch = 0;

    // Total triangle intersection tests.
    std::uint64_t totalBruteForceTests = 0;
    std::uint64_t totalBVHTests = 0;

    for (const Ray& ray : rays) {

        HitRecord bruteRecord;
        HitRecord bvhRecord;

        TraversalStats bruteStats;
        TraversalStats bvhStats;

        // Run Brute Force.
        const bool bruteHit = bruteForceHit(
            objects,
            ray,
            rayInterval,
            bruteRecord,
            &bruteStats
        );

        // Run BVH with statistics.
        const bool bvhHit = bvh.hit(
            ray,
            rayInterval,
            bvhRecord,
            bvhStats
        );

        // Accumulate intersection test counts.
        totalBruteForceTests += bruteStats.intersectionTests;
        totalBVHTests += bvhStats.intersectionTests;

        // Compare hit/miss results.
        if (bruteHit != bvhHit) {
            ++hitMismatch;
            continue;
        }

        if (!bruteHit) {
            continue;
        }

        // Compare intersection distances.
        const double tScale = std::max({
            1.0,
            std::abs(bruteRecord.t),
            std::abs(bvhRecord.t)
        });

        if (std::abs(bruteRecord.t - bvhRecord.t) > 1e-8 * tScale) {
            ++tMismatch;
        }

        // Compare intersection points.
        const Vec3 pointDifference =
            bruteRecord.intersectionPoint - bvhRecord.intersectionPoint;

        const double pointScale = std::max({
            1.0,
            bruteRecord.intersectionPoint.length(),
            bvhRecord.intersectionPoint.length()
        });

        if (pointDifference.length() > 1e-7 * pointScale) {
            ++pointMismatch;
        }
    }

    // Print validation results.
    std::cout << "\n===== BVH VALIDATION =====\n";
    std::cout << "Total rays: " << rays.size() << '\n';
    std::cout << "Hit mismatches: " << hitMismatch << '\n';
    std::cout << "t mismatches: " << tMismatch << '\n';
    std::cout << "Point mismatches: " << pointMismatch << '\n';

    const bool passed =
        hitMismatch == 0 &&
        tMismatch == 0 &&
        pointMismatch == 0;

    std::cout << "Validation: "
              << (passed ? "PASSED" : "FAILED")
              << '\n';

    // Print performance comparison.
    std::cout << "\n===== PERFORMANCE COMPARISON =====\n";

    std::cout << "Brute Force triangle tests: "
              << totalBruteForceTests << '\n';

    std::cout << "BVH triangle tests: "
              << totalBVHTests << '\n';

    if (totalBruteForceTests > 0) {

        const double reduction =
            100.0 * (
                1.0 -
                static_cast<double>(totalBVHTests) /
                static_cast<double>(totalBruteForceTests)
            );

        std::cout << "Triangle test reduction: "
                  << reduction << "%\n";
    }
}


// Runs the BVH validation test.
inline void runBVHTest()
{
    const auto material = createBVHTestMaterial();

    std::vector<std::shared_ptr<Hittable>> objects;

    // Triangle 1: Center, z = 0
    objects.push_back(std::make_shared<Triangle>(
        Point3{-1.0, -1.0, 0.0},
        Point3{1.0, -1.0, 0.0},
        Point3{0.0, 1.0, 0.0},
        material
    ));

    // Triangle 2: Behind Triangle 1, z = -2
    objects.push_back(std::make_shared<Triangle>(
        Point3{-1.0, -1.0, -2.0},
        Point3{1.0, -1.0, -2.0},
        Point3{0.0, 1.0, -2.0},
        material
    ));

    // Triangle 3: Right side
    objects.push_back(std::make_shared<Triangle>(
        Point3{2.0, -1.0, 0.0},
        Point3{4.0, -1.0, 0.0},
        Point3{3.0, 1.0, 0.0},
        material
    ));

    // Triangle 4: Left side
    objects.push_back(std::make_shared<Triangle>(
        Point3{-4.0, -1.0, 0.0},
        Point3{-2.0, -1.0, 0.0},
        Point3{-3.0, 1.0, 0.0},
        material
    ));

    std::vector<Ray> rays;

    // Should hit the center triangle.
    rays.emplace_back(
        Point3{0.0, 0.0, 5.0},
        Vec3{0.0, 0.0, -1.0}
    );

    // Should hit the right triangle.
    rays.emplace_back(
        Point3{3.0, 0.0, 5.0},
        Vec3{0.0, 0.0, -1.0}
    );

    // Should hit the left triangle.
    rays.emplace_back(
        Point3{-3.0, 0.0, 5.0},
        Vec3{0.0, 0.0, -1.0}
    );

    // Should miss all triangles.
    rays.emplace_back(
        Point3{7.0, 0.0, 5.0},
        Vec3{0.0, 0.0, -1.0}
    );

    // Should point away from all triangles.
    rays.emplace_back(
        Point3{0.0, 0.0, 5.0},
        Vec3{0.0, 0.0, 1.0}
    );

    // Tests the nearest hit from below.
    rays.emplace_back(
        Point3{0.0, 0.0, -4.0},
        Vec3{0.0, 0.0, 1.0}
    );

    // Validate BVH against Brute Force.
    validateBVH(
        objects,
        rays,
        Interval{0.001, 1000.0},
        1
    );

    // Test traversal statistics.
    BVH bvh(objects, 1);

    std::cout << "\n===== TRAVERSAL STATISTICS =====\n";

    for (std::size_t i = 0; i < rays.size(); ++i) {

        TraversalStats stats;
        HitRecord record;

        const bool hit = bvh.hit(
            rays[i],
            Interval{0.001, 1000.0},
            record,
            stats
        );

        std::cout << "\nRay " << i + 1 << '\n';

        std::cout << "Hit: "
                  << (hit ? "Yes" : "No") << '\n';

        std::cout << "Traversal steps: "
                  << stats.traversalSteps << '\n';

        std::cout << "Bounds tests: "
                  << stats.boundsTests << '\n';

        std::cout << "Triangle tests: "
                  << stats.intersectionTests << '\n';
    }
}