#ifndef SIMPLE_RENDERER_BVH_DAVID_H
#define SIMPLE_RENDERER_BVH_DAVID_H

#include "../Scene/Scene.h"
#include "../Types/Vec3.h"

struct BVHNode {
    Vec3 aabbMin, aabbMax;
    uint leftChild, rightChild;
    bool isLeaf;
    uint firstPrim, lastPrim;
} typedef BVHNode;

class BVH {
public:
    // Constructor
    explicit BVH(const Scene &scene)
        : totalPrimitiveCount_(scene.triangleCount()),
          bvhNodes_(totalPrimitiveCount_) {
        // Build the BVH
        BuildBVH(scene);
    }

private:
    const uint rootNodeIdx_ = 0, nodesUsed_ = 1;
    const uint totalPrimitiveCount_;
    std::vector<BVHNode> bvhNodes_;

    void BuildBVH(const Scene &scene) {
        scene.
                BVHNode * root = bvhNodes_[rootNodeIdx_];
    }
};

void BuildBVH() {
    for (int i = 0; i < 3; i++) {
    }
}

#endif //SIMPLE_RENDERER_BVH_DAVID_H
