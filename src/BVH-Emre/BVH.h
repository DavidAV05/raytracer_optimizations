
#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include "TraversalStats.h"
#include "../Intersections/Hittable.h"
#include "../Types/Vec3.h"
#include <array>
#include <limits>

enum class BVHSplitMethod
{
    Median,
    SAH
};
class BVH final : public Hittable {
private:

    // Information about each primitive (triangle).
    struct PrimitiveInfo {
        std::shared_ptr<Hittable> object;
        Aabb bounds;
        Point3 centroid;
    };

    // Represents one node in the binary BVH tree.
    struct Node {
        Aabb bounds;

        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        // Range of primitives stored in a leaf.  Ağaç yapısı
        std::size_t begin = 0;
        std::size_t end = 0;

        [[nodiscard]] bool isLeaf() const noexcept {
            return !left && !right;
        }
    };

    // Returns the selected coordinate of a Vec3.
    static double component(const Vec3& value, int axis) noexcept {
        if (axis == 0) return value.x();
        if (axis == 1) return value.y();
        return value.z();
    }

    std::vector<PrimitiveInfo> primitives_;

    std::unique_ptr<Node> root_;

    std::size_t maxLeafSize_;

    BVHSplitMethod splitMethod_;

    [[nodiscard]] static double surfaceArea(const Aabb& bounds)  //SAH HELPER = Yüzey Alanı Hesaplıyor
    {
        const Vec3 extent =
            bounds.maximum() - bounds.minimum();

        const double x = std::max(0.0, extent.x());
        const double y = std::max(0.0, extent.y());
        const double z = std::max(0.0, extent.z());

        return 2.0 * (x * y + y * z + z * x);
    }

public:

    // We will implement this constructor in the build step. CONSTRUCTOR
    explicit BVH(
        std::vector<std::shared_ptr<Hittable>> objects,
        std::size_t maxLeafSize = 4,
        BVHSplitMethod splitMethod = BVHSplitMethod::Median
    )
        : maxLeafSize_(maxLeafSize),
          splitMethod_(splitMethod)
    {
        if (maxLeafSize_ == 0) {
            throw std::invalid_argument(
                "maxLeafSize must be greater than zero"
            );
        }

        primitives_.reserve(objects.size());

        // Collect bounding boxes and centroids.
        for (auto& object : objects) {

            if (!object) {
                throw std::invalid_argument(
                    "BVH cannot contain a null object"
                );
            }

            Aabb bounds;

            if (!object->boundingBox(bounds)) {
                throw std::invalid_argument(
                    "BVH objects must provide a bounding box"
                );
            }

            const Point3 centroid =
                (bounds.minimum() + bounds.maximum()) / 2.0;

            primitives_.push_back({
                std::move(object),
                bounds,
                centroid
            });
        }

        // Build the root node.
        if (!primitives_.empty()) {
            root_ = build(0, primitives_.size());
        }
    }

    // Temporary placeholder.
    // We will implement ray traversal in the next step. TRAVERSAL PART  (HİT PART DEĞİŞTİRİLEBİLİR SONRASINDA OVERRİDE EDİLECEK ŞEKİLDE)
    // Standard hit function required by the Hittable interface.
    [[nodiscard]] bool hit(
        const Ray& ray,
        Interval interval,
        HitRecord& record
    ) const override {

        if (!root_) {
            return false;
        }

        return hitNode(
            *root_,
            ray,
            interval,
            record,
            nullptr
        );
    }


    // Hit function with traversal statistics.
    [[nodiscard]] bool hit(
        const Ray& ray,
        Interval interval,
        HitRecord& record,
        TraversalStats& stats
    ) const {

        stats.reset();

        if (!root_) {
            return false;
        }

        return hitNode(
            *root_,
            ray,
            interval,
            record,
            &stats
        );
    }
    // Returns the bounding box of the complete BVH.
    [[nodiscard]] bool boundingBox(Aabb& output) const override {

        if (!root_) {
            return false;
        }

        output = root_->bounds;
        return true;
    }

private:



[[nodiscard]] bool hitNode(
    const Node& node,
    const Ray& ray,
    Interval interval,
    HitRecord& record,
    TraversalStats* stats
) const {

    // STEP 1: Count the visited node.
    if (stats) {
        ++stats->traversalSteps;
    }

    // STEP 2: Count and test the node's bounding box.
    if (stats) {
        ++stats->boundsTests;
    }

    if (!node.bounds.hit(ray, interval)) {
        return false;
    }

    // STEP 3: If this is a leaf, test its triangles.
    if (node.isLeaf()) {

        bool hitAnything = false;
        double closestSoFar = interval.max();

        HitRecord tempRecord;

        for (std::size_t i = node.begin; i < node.end; ++i) {

            const Interval searchInterval{
                interval.min(),
                closestSoFar
            };

            // Count each primitive intersection attempt.
            if (stats) {
                ++stats->intersectionTests;
            }

            if (primitives_[i].object->hit(
                ray,
                searchInterval,
                tempRecord
            )) {

                hitAnything = true;
                closestSoFar = tempRecord.t;
                record = tempRecord;
            }
        }

        return hitAnything;
    }

    // STEP 4: Traverse the child nodes.
    bool hitAnything = false;
    double closestSoFar = interval.max();

    HitRecord childRecord;

    // Check the left child.
    if (node.left) {

        const Interval leftInterval{
            interval.min(),
            closestSoFar
        };

        if (hitNode(
            *node.left,
            ray,
            leftInterval,
            childRecord,
            stats
        )) {

            hitAnything = true;
            closestSoFar = childRecord.t;
            record = childRecord;
        }
    }

    // Check the right child.
    if (node.right) {

        const Interval rightInterval{
            interval.min(),
            closestSoFar
        };

        if (hitNode(
            *node.right,
            ray,
            rightInterval,
            childRecord,
            stats
        )) {

            hitAnything = true;
            closestSoFar = childRecord.t;
            record = childRecord;
        }
    }

    return hitAnything;
}
    // Recursively builds a subtree using Median Split.

// Recursively builds a subtree using Median or SAH split.
[[nodiscard]] std::unique_ptr<Node> build(
    std::size_t begin,
    std::size_t end
) {
    // Create a new node.
    auto node = std::make_unique<Node>();

    node->begin = begin;
    node->end = end;

    // Calculate the bounding box of this node.
    Aabb nodeBounds;

    for (std::size_t i = begin; i < end; ++i) {
        nodeBounds.include(primitives_[i].bounds);
    }

    node->bounds = nodeBounds;

    const std::size_t count = end - begin;

    // Stop splitting when the maximum leaf size is reached.
    if (count <= maxLeafSize_) {
        return node;
    }

    // Find the minimum and maximum centroid coordinates.
    Point3 centroidMin = primitives_[begin].centroid;
    Point3 centroidMax = primitives_[begin].centroid;

    for (std::size_t i = begin + 1; i < end; ++i) {
        const Point3& centroid = primitives_[i].centroid;

        centroidMin = Point3{
            std::min(centroidMin.x(), centroid.x()),
            std::min(centroidMin.y(), centroid.y()),
            std::min(centroidMin.z(), centroid.z())
        };

        centroidMax = Point3{
            std::max(centroidMax.x(), centroid.x()),
            std::max(centroidMax.y(), centroid.y()),
            std::max(centroidMax.z(), centroid.z())
        };
    }

    // Calculate centroid distribution along each axis.
    const Vec3 extent = centroidMax - centroidMin;

    // Select the axis with the largest centroid extent.
    int medianAxis = 0;

    if (extent.y() > extent.x()) {
        medianAxis = 1;
    }

    if (extent.z() > component(extent, medianAxis)) {
        medianAxis = 2;
    }

    // Default split position.
    std::size_t mid = begin + count / 2;

    bool sahPartitioned = false;

    // =====================================================
    // SAH SPLIT
    // =====================================================

    if (splitMethod_ == BVHSplitMethod::SAH) {

        constexpr std::size_t bucketCount = 12;

        struct Bucket {
            std::size_t count = 0;
            Aabb bounds;
        };

        const double parentArea = surfaceArea(nodeBounds);

        double bestCost = std::numeric_limits<double>::infinity();

        int bestAxis = -1;
        int bestBucket = -1;

        double bestCentroidMin = 0.0;
        double bestCentroidExtent = 0.0;

        // SAH requires a valid parent surface area.
        if (parentArea > 0.0) {

            // Try all three axes.
            for (int axis = 0; axis < 3; ++axis) {

                const double minCentroid = component(centroidMin, axis);
                const double maxCentroid = component(centroidMax, axis);

                const double axisExtent = maxCentroid - minCentroid;

                // Skip an axis with no centroid variation.
                if (axisExtent <= 0.0) {
                    continue;
                }

                std::array<Bucket, bucketCount> buckets{};

                // Map each primitive to one of the buckets.
                const auto getBucketIndex =
                    [axis, minCentroid, axisExtent](const PrimitiveInfo& primitive) {

                        const double normalized = std::clamp(
                            (component(primitive.centroid, axis) - minCentroid)
                                / axisExtent,
                            0.0,
                            1.0
                        );

                        const std::size_t index =
                            static_cast<std::size_t>(normalized * bucketCount);

                        return std::min(index, bucketCount - 1);
                    };

                // Populate the buckets.
                for (std::size_t i = begin; i < end; ++i) {

                    const PrimitiveInfo& primitive = primitives_[i];

                    const std::size_t bucketIndex = getBucketIndex(primitive);

                    Bucket& bucket = buckets[bucketIndex];

                    if (bucket.count == 0) {
                        bucket.bounds = primitive.bounds;
                    }
                    else {
                        bucket.bounds.include(primitive.bounds);
                    }

                    ++bucket.count;
                }

                // Evaluate all possible bucket split positions.
                for (std::size_t split = 0; split < bucketCount - 1; ++split) {

                    std::size_t leftCount = 0;
                    std::size_t rightCount = 0;

                    Aabb leftBounds;
                    Aabb rightBounds;

                    bool hasLeftBounds = false;
                    bool hasRightBounds = false;

                    // Collect primitives on both sides of the split.
                    for (std::size_t i = 0; i < bucketCount; ++i) {

                        if (buckets[i].count == 0) {
                            continue;
                        }

                        if (i <= split) {

                            leftCount += buckets[i].count;

                            if (!hasLeftBounds) {
                                leftBounds = buckets[i].bounds;
                                hasLeftBounds = true;
                            }
                            else {
                                leftBounds.include(buckets[i].bounds);
                            }
                        }
                        else {

                            rightCount += buckets[i].count;

                            if (!hasRightBounds) {
                                rightBounds = buckets[i].bounds;
                                hasRightBounds = true;
                            }
                            else {
                                rightBounds.include(buckets[i].bounds);
                            }
                        }
                    }

                    // Both children must contain primitives.
                    if (leftCount == 0 || rightCount == 0) {
                        continue;
                    }

                    // SAH cost:
                    // Traversal cost + weighted intersection costs.
                    const double cost =
                        1.0 +
                        (
                            surfaceArea(leftBounds) * static_cast<double>(leftCount) +
                            surfaceArea(rightBounds) * static_cast<double>(rightCount)
                        ) / parentArea;

                    // Keep the cheapest split.
                    if (cost < bestCost) {

                        bestCost = cost;
                        bestAxis = axis;
                        bestBucket = static_cast<int>(split);

                        bestCentroidMin = minCentroid;
                        bestCentroidExtent = axisExtent;
                    }
                }
            }
        }

        // Apply the best SAH split.
        if (bestAxis != -1) {

            const auto getBestBucketIndex =
                [bestAxis, bestCentroidMin, bestCentroidExtent]
                (const PrimitiveInfo& primitive) {

                    constexpr std::size_t bucketCount = 12;

                    const double normalized = std::clamp(
                        (component(primitive.centroid, bestAxis) - bestCentroidMin)
                            / bestCentroidExtent,
                        0.0,
                        1.0
                    );

                    const std::size_t index =
                        static_cast<std::size_t>(normalized * bucketCount);

                    return std::min(index, bucketCount - 1);
                };

            auto first = primitives_.begin() + begin;
            auto last = primitives_.begin() + end;

            // Partition primitives according to the selected bucket.
            auto middle = std::partition(
                first,
                last,
                [&getBestBucketIndex, bestBucket](const PrimitiveInfo& primitive) {
                    return getBestBucketIndex(primitive)
                        <= static_cast<std::size_t>(bestBucket);
                }
            );

            mid = begin + static_cast<std::size_t>(middle - first);

            sahPartitioned = (mid > begin && mid < end);
        }
    }

    // =====================================================
    // MEDIAN SPLIT / FALLBACK
    // =====================================================

    // Use Median if:
    // 1. Median was selected, or
    // 2. SAH could not find a valid partition.
    if (!sahPartitioned) {

        mid = begin + count / 2;

        std::nth_element(
            primitives_.begin() + begin,
            primitives_.begin() + mid,
            primitives_.begin() + end,

            [medianAxis](const PrimitiveInfo& a, const PrimitiveInfo& b) {
                return BVH::component(a.centroid, medianAxis)
                     < BVH::component(b.centroid, medianAxis);
            }
        );
    }

    // Recursively build left and right children.
    node->left = build(begin, mid);
    node->right = build(mid, end);

    return node;
}
};
