#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <chrono>
#include "BVHMeshBenchmark.h"
#include "TraversalStats.h"

#include "../Intersections/Ray.h"
#include "../Intersections/Hittable.h"
#include "../Types/Interval.h"
#include "../Types/Vec3.h"


// Camera viewpoint information.
struct CameraView
{
    std::string name;
    Point3 position;
};


// Stores minimum, maximum and average statistics.
struct MetricSummary
{
    std::uint64_t total = 0;

    std::uint64_t minimum =
        std::numeric_limits<std::uint64_t>::max();

    std::uint64_t maximum = 0;

    void add(const std::uint64_t value)
    {
        total += value;

        minimum = std::min(minimum, value);
        maximum = std::max(maximum, value);
    }

    [[nodiscard]] double average(
        const std::size_t count
    ) const
    {
        if (count == 0) {
            return 0.0;
        }

        return static_cast<double>(total) /
               static_cast<double>(count);
    }
};


// Stores benchmark results for one camera.
struct ViewBenchmarkResult
{
    std::string bvhName;
    std::string cameraName;

    std::size_t totalRays = 0;
    std::size_t hitRays = 0;
    std::size_t missRays = 0;

    MetricSummary traversalSteps;
    MetricSummary boundsTests;
    MetricSummary intersectionTests;
};

// Generates primary rays for a pinhole camera.
inline std::vector<Ray> generatePrimaryRays(
    const Point3& cameraPosition,
    const Point3& target,
    const std::size_t imageWidth,
    const std::size_t imageHeight,
    const double verticalFovDegrees
) {
    constexpr double pi = 3.14159265358979323846;

    const double aspectRatio =
        static_cast<double>(imageWidth) /
        static_cast<double>(imageHeight);

    const double fovRadians =
        verticalFovDegrees * pi / 180.0;

    const double halfHeight =
        std::tan(fovRadians / 2.0);

    const double halfWidth =
        aspectRatio * halfHeight;

    const Vec3 forward =
        (target - cameraPosition).normalized();

    Vec3 worldUp{0.0, 1.0, 0.0};

    // Avoid a degenerate camera basis.
    if (std::abs(forward.dot(worldUp)) > 0.99) {
        worldUp = Vec3{0.0, 0.0, 1.0};
    }

    const Vec3 right =
        forward.cross(worldUp).normalized();

    const Vec3 cameraUp =
        right.cross(forward).normalized();

    std::vector<Ray> rays;

    rays.reserve(imageWidth * imageHeight);

    for (std::size_t y = 0; y < imageHeight; ++y) {

        for (std::size_t x = 0; x < imageWidth; ++x) {

            const double u =
                (static_cast<double>(x) + 0.5) /
                static_cast<double>(imageWidth);

            const double v =
                (static_cast<double>(y) + 0.5) /
                static_cast<double>(imageHeight);

            const double horizontal =
                (2.0 * u - 1.0) * halfWidth;

            const double vertical =
                (1.0 - 2.0 * v) * halfHeight;

            const Vec3 direction = (
                forward +
                right * horizontal +
                cameraUp * vertical
            ).normalized();

            rays.emplace_back(
                cameraPosition,
                direction
            );
        }
    }

    return rays;
}


inline ViewBenchmarkResult benchmarkCamera(
    const BVH& bvh,
    const CameraView& camera,
    const std::vector<Ray>& rays,
    std::ofstream& rayCsv,
    const std::string& bvhName
) {
    ViewBenchmarkResult result;

    result.bvhName = bvhName;
    result.cameraName = camera.name;
    result.totalRays = rays.size();

    std::cout << "\nRunning benchmark: "
              << bvhName << " / "
              << camera.name << '\n';

    for (std::size_t i = 0; i < rays.size(); ++i) {

        TraversalStats stats;
        HitRecord record;

        const bool hit = bvh.hit(
            rays[i],
            Interval{
                0.001,
                std::numeric_limits<double>::infinity()
            },
            record,
            stats
        );

        if (hit) {
            ++result.hitRays;
        }
        else {
            ++result.missRays;
        }

        result.traversalSteps.add(stats.traversalSteps);
        result.boundsTests.add(stats.boundsTests);
        result.intersectionTests.add(stats.intersectionTests);

        // Save statistics for each individual ray.
        rayCsv
            << bvhName << ','
            << camera.name << ','
            << i << ','
            << (hit ? 1 : 0) << ','
            << stats.traversalSteps << ','
            << stats.boundsTests << ','
            << stats.intersectionTests
            << '\n';
    }

    return result;
}
inline std::size_t countBVHMismatches(
    const BVH& firstBVH,
    const BVH& secondBVH,
    const std::vector<Ray>& rays
) {
    std::size_t mismatches = 0;

    for (const Ray& ray : rays) {

        HitRecord firstRecord;
        HitRecord secondRecord;

        const Interval interval{
            0.001,
            std::numeric_limits<double>::infinity()
        };

        const bool firstHit =
            firstBVH.hit(ray, interval, firstRecord);

        const bool secondHit =
            secondBVH.hit(ray, interval, secondRecord);

        if (firstHit != secondHit) {
            ++mismatches;
            continue;
        }

        if (firstHit) {

            const double scale = std::max(
                1.0,
                std::max(
                    std::abs(firstRecord.t),
                    std::abs(secondRecord.t)
                )
            );

            const double tolerance = 1e-6 * scale;

            if (std::abs(firstRecord.t - secondRecord.t) > tolerance) {
                ++mismatches;
            }
        }
    }

    return mismatches;
}

// Prints the statistics of one camera.
inline void printBenchmarkResult(
    const ViewBenchmarkResult& result
) {
    std::cout << "\n===== "
              << result.bvhName << " / "
              << result.cameraName
              << " RESULTS =====\n";

    std::cout << "Total rays: "
              << result.totalRays << '\n';

    std::cout << "Hit rays: "
              << result.hitRays << '\n';

    std::cout << "Miss rays: "
              << result.missRays << '\n';

    std::cout << std::fixed << std::setprecision(3);

    std::cout << "\nTraversal steps:\n";

    std::cout << "Average: "
              << result.traversalSteps.average(result.totalRays)
              << '\n';

    std::cout << "Minimum: "
              << result.traversalSteps.minimum
              << '\n';

    std::cout << "Maximum: "
              << result.traversalSteps.maximum
              << '\n';

    std::cout << "\nBounds tests:\n";

    std::cout << "Average: "
              << result.boundsTests.average(result.totalRays)
              << '\n';

    std::cout << "Minimum: "
              << result.boundsTests.minimum
              << '\n';

    std::cout << "Maximum: "
              << result.boundsTests.maximum
              << '\n';

    std::cout << "\nTriangle intersection tests:\n";

    std::cout << "Average: "
              << result.intersectionTests.average(result.totalRays)
              << '\n';

    std::cout << "Minimum: "
              << result.intersectionTests.minimum
              << '\n';

    std::cout << "Maximum: "
              << result.intersectionTests.maximum
              << '\n';
}


// Saves summary statistics to a CSV file.
inline void writeSummaryCsv(
    const std::vector<ViewBenchmarkResult>& results,
    const std::string& path
) {
    std::ofstream csv(path);

    if (!csv) {
        throw std::runtime_error(
            "Could not create summary CSV: " + path
        );
    }

    csv << "BVH,Camera,TotalRays,HitRays,MissRays,"
        << "AvgTraversalSteps,MinTraversalSteps,MaxTraversalSteps,"
        << "AvgBoundsTests,MinBoundsTests,MaxBoundsTests,"
        << "AvgTriangleTests,MinTriangleTests,MaxTriangleTests\n";

    csv << std::fixed << std::setprecision(6);

    for (const auto& result : results) {

        csv
    << result.bvhName << ','
    << result.cameraName << ','
    << result.totalRays << ','
    << result.hitRays << ','
    << result.missRays << ','

    << result.traversalSteps.average(result.totalRays) << ','
    << result.traversalSteps.minimum << ','
    << result.traversalSteps.maximum << ','

    << result.boundsTests.average(result.totalRays) << ','
    << result.boundsTests.minimum << ','
    << result.boundsTests.maximum << ','

    << result.intersectionTests.average(result.totalRays) << ','
    << result.intersectionTests.minimum << ','
    << result.intersectionTests.maximum

    << '\n';
    }
}



inline void runBVHCameraBenchmark(
    const std::string& meshPath
) {
    constexpr std::size_t imageWidth = 160;
    constexpr std::size_t imageHeight = 120;
    constexpr double verticalFov = 45.0;
    constexpr std::size_t maxLeafSize = 4;

    const auto material = createBVHTestMaterial();

    // Load and subdivide the OBJ mesh only once.
    const auto objects = loadSubdividedMeshForBVH(
        meshPath,
        material,
        4
    );

    std::cout << "\nTriangle count: "
              << objects.size() << '\n';

    // =============================================
    // BUILD MEDIAN BVH
    // =============================================

    std::cout << "\nBuilding Median Split BVH...\n";

    // Copy the shared pointers before starting the timer.
    auto medianObjects = objects;

    const auto medianBuildStart =
        std::chrono::steady_clock::now();

    const BVH medianBVH(
        std::move(medianObjects),
        maxLeafSize,
        BVHSplitMethod::Median
    );

    const auto medianBuildEnd =
        std::chrono::steady_clock::now();

    const double medianBuildMs =
        std::chrono::duration<double, std::milli>(
            medianBuildEnd - medianBuildStart
        ).count();

    // =============================================
    // BUILD SAH BVH
    // =============================================

    std::cout << "Building SAH BVH...\n";

    auto sahObjects = objects;

    const auto sahBuildStart =
        std::chrono::steady_clock::now();

    const BVH sahBVH(
        std::move(sahObjects),
        maxLeafSize,
        BVHSplitMethod::SAH
    );

    const auto sahBuildEnd =
        std::chrono::steady_clock::now();

    const double sahBuildMs =
        std::chrono::duration<double, std::milli>(
            sahBuildEnd - sahBuildStart
        ).count();

    std::cout << "Both BVHs constructed successfully.\n";

    // =============================================
    // CAMERA VIEWPOINTS
    // =============================================

    const std::vector<CameraView> cameras{
        {"Front", Point3{0.0, 0.0, 5.0}},
        {"Side", Point3{5.0, 0.0, 0.0}},
        {"Diagonal", Point3{3.0, 2.0, 3.0}}
    };

    // =============================================
    // GENERATE RAYS ONLY ONCE
    // =============================================

    std::vector<std::vector<Ray>> cameraRays;

    cameraRays.reserve(cameras.size());

    for (const CameraView& camera : cameras) {

        cameraRays.push_back(
            generatePrimaryRays(
                camera.position,
                Point3{0.0, 0.0, 0.0},
                imageWidth,
                imageHeight,
                verticalFov
            )
        );
    }

    std::cout << "\n===== RAY DATASET =====\n";

    std::size_t totalRayCount = 0;

    for (std::size_t i = 0; i < cameras.size(); ++i) {

        std::cout << cameras[i].name << ": "
                  << cameraRays[i].size() << " rays\n";

        totalRayCount += cameraRays[i].size();
    }

    std::cout << "Total rays: "
              << totalRayCount << '\n';

    // =============================================
    // VALIDATE MEDIAN AGAINST SAH
    // =============================================

    std::cout << "\n===== BVH CORRECTNESS CHECK =====\n";

    for (std::size_t i = 0; i < cameras.size(); ++i) {

        const std::size_t mismatches =
            countBVHMismatches(
                medianBVH,
                sahBVH,
                cameraRays[i]
            );

        std::cout << cameras[i].name
                  << " mismatches: "
                  << mismatches << '\n';

        if (mismatches != 0) {
            throw std::runtime_error(
                "Median and SAH BVHs produced different hit results."
            );
        }
    }

    std::cout << "Correctness check: PASSED\n";

    // =============================================
    // SAVE BUILD TIMES
    // =============================================

    std::ofstream buildCsv(
        "BVH-Emre/bvh_build_times.csv"
    );

    if (!buildCsv) {
        throw std::runtime_error(
            "Could not create BVH build time CSV."
        );
    }

    buildCsv
        << "BVH,TriangleCount,BuildTimeMilliseconds\n";

    buildCsv << std::fixed << std::setprecision(6);

    buildCsv
        << "Median,"
        << objects.size() << ','
        << medianBuildMs << '\n';

    buildCsv
        << "SAH,"
        << objects.size() << ','
        << sahBuildMs << '\n';

    buildCsv.close();

    std::cout << "\n===== BUILD TIMES =====\n";

    std::cout << "Median: "
              << medianBuildMs << " ms\n";

    std::cout << "SAH: "
              << sahBuildMs << " ms\n";

    // =============================================
    // OPEN PER-RAY CSV
    // =============================================

    std::ofstream rayCsv(
        "BVH-Emre/bvh_comparison_ray_stats.csv"
    );

    if (!rayCsv) {
        throw std::runtime_error(
            "Could not create per-ray comparison CSV."
        );
    }

    rayCsv
        << "BVH,Camera,RayIndex,Hit,TraversalSteps,"
        << "BoundsTests,TriangleTests\n";

    // =============================================
    // BENCHMARK MEDIAN BVH
    // =============================================

    std::vector<ViewBenchmarkResult> results;

    std::cout << "\n===== MEDIAN BVH BENCHMARK =====\n";

    for (std::size_t i = 0; i < cameras.size(); ++i) {

        const ViewBenchmarkResult result =
            benchmarkCamera(
                medianBVH,
                cameras[i],
                cameraRays[i],
                rayCsv,
                "Median"
            );

        printBenchmarkResult(result);

        results.push_back(result);
    }

    // =============================================
    // BENCHMARK SAH BVH
    // =============================================

    std::cout << "\n===== SAH BVH BENCHMARK =====\n";

    for (std::size_t i = 0; i < cameras.size(); ++i) {

        const ViewBenchmarkResult result =
            benchmarkCamera(
                sahBVH,
                cameras[i],
                cameraRays[i],
                rayCsv,
                "SAH"
            );

        printBenchmarkResult(result);

        results.push_back(result);
    }

    rayCsv.close();

    // =============================================
    // SAVE SUMMARY CSV
    // =============================================

    writeSummaryCsv(
        results,
        "BVH-Emre/bvh_comparison_summary.csv"
    );

    std::cout << "\n===== COMPARISON COMPLETED =====\n";

    std::cout << "Per-ray results: "
              << "BVH-Emre/bvh_comparison_ray_stats.csv\n";

    std::cout << "Summary results: "
              << "BVH-Emre/bvh_comparison_summary.csv\n";

    std::cout << "Build times: "
              << "BVH-Emre/bvh_build_times.csv\n";
}
