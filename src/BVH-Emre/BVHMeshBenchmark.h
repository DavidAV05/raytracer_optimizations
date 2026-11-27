#pragma once

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "BVH.h"
#include "BVHValidation.h"
#include "../Scene/ObjLoader.h"

inline std::vector<std::shared_ptr<Hittable>> loadSubdividedMeshForBVH(
    const std::string& meshPath,
    std::shared_ptr<const Material> material,
    std::size_t subdivisionsPerEdge = 4
) {
    if (!material) {
        throw std::invalid_argument("Material must not be null.");
    }

    if (subdivisionsPerEdge == 0) {
        throw std::invalid_argument("Subdivision count must be positive.");
    }

    // Read the original OBJ file.
    std::ifstream input(meshPath);

    if (!input) {
        throw std::runtime_error(
            "Could not open OBJ file: " + meshPath
        );
    }

    std::ostringstream objText;
    std::string line;

    while (std::getline(input, line)) {
        std::istringstream row(line);
        std::string directive;
        row >> directive;

        if (directive != "mtllib" && directive != "usemtl") {
            objText << line << '\n';
        }
    }

    // Parse the OBJ file.
    tinyobj::ObjReaderConfig config;
    config.triangulate = true;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromString(objText.str(), "", config)) {
        throw std::runtime_error(
            "Could not parse OBJ: " + reader.Error()
        );
    }

    const auto& attributes = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();

    std::vector<std::shared_ptr<Hittable>> objects;

    std::size_t originalTriangleCount = 0;

    // Process every triangle in the OBJ.
    for (const auto& shape : shapes) {

        std::size_t indexOffset = 0;

        for (const unsigned int verticesInFace :
             shape.mesh.num_face_vertices) {

            if (verticesInFace != 3) {
                throw std::runtime_error(
                    "Expected triangulated OBJ faces."
                );
            }

            Point3 points[3];

            for (std::size_t corner = 0; corner < 3; ++corner) {

                const int vertexIndex =
                    shape.mesh.indices[indexOffset + corner].vertex_index;

                if (vertexIndex < 0 ||
                    attributes.vertices.size() < 3 ||
                    static_cast<std::size_t>(vertexIndex) >
                    (attributes.vertices.size() - 3) / 3) {

                    throw std::runtime_error(
                        "Invalid OBJ vertex index."
                    );
                }

                const std::size_t offset =
                    static_cast<std::size_t>(vertexIndex) * 3;

                points[corner] = Point3{
                    attributes.vertices[offset],
                    attributes.vertices[offset + 1],
                    attributes.vertices[offset + 2]
                };
            }

            ++originalTriangleCount;

            const Point3 a = points[0];
            const Point3 b = points[1];
            const Point3 c = points[2];

            // Generates a point using barycentric coordinates.
            const auto pointAt = [&](std::size_t i, std::size_t j) {

                const double u =
                    static_cast<double>(i) / subdivisionsPerEdge;

                const double v =
                    static_cast<double>(j) / subdivisionsPerEdge;

                return Point3{
                    a.x() + (b.x() - a.x()) * u
                          + (c.x() - a.x()) * v,

                    a.y() + (b.y() - a.y()) * u
                          + (c.y() - a.y()) * v,

                    a.z() + (b.z() - a.z()) * u
                          + (c.z() - a.z()) * v
                };
            };

            // Subdivide the original triangle.
            for (std::size_t i = 0;
                 i < subdivisionsPerEdge;
                 ++i) {

                for (std::size_t j = 0;
                     j < subdivisionsPerEdge - i;
                     ++j) {

                    const Point3 p00 = pointAt(i, j);
                    const Point3 p10 = pointAt(i + 1, j);
                    const Point3 p01 = pointAt(i, j + 1);

                    // First triangle.
                    objects.push_back(
                        std::make_shared<Triangle>(
                            p00, p10, p01, material
                        )
                    );

                    // Second triangle, when the cell allows it.
                    if (i + j + 1 < subdivisionsPerEdge) {

                        const Point3 p11 =
                            pointAt(i + 1, j + 1);

                        objects.push_back(
                            std::make_shared<Triangle>(
                                p10, p11, p01, material
                            )
                        );
                    }
                }
            }

            indexOffset += verticesInFace;
        }
    }

    std::cout << "\n===== MESH SUBDIVISION =====\n";

    std::cout << "Original triangles: "
              << originalTriangleCount << '\n';

    std::cout << "Subdivisions per edge: "
              << subdivisionsPerEdge << '\n';

    std::cout << "Triangles after subdivision: "
              << objects.size() << '\n';

    if (objects.size() < 10000) {
        throw std::runtime_error(
            "Mesh still contains fewer than 10,000 triangles."
        );
    }

    return objects;
}


// Loads the refined mesh and builds a binary Median Split BVH.
inline void runBVHMeshLoadTest(const std::string& meshPath)
{
    const auto material = createBVHTestMaterial();

    const auto objects = loadSubdividedMeshForBVH(
        meshPath,
        material,
        4
    );

    BVH bvh(objects, 4);

    Aabb meshBounds;

    if (!bvh.boundingBox(meshBounds)) {
        throw std::runtime_error(
            "Could not retrieve BVH bounding box."
        );
    }

    std::cout << "\n===== BVH BUILD TEST =====\n";

    std::cout << "BVH construction: SUCCESS\n";

    std::cout << "Triangles in BVH: "
              << objects.size() << '\n';

    std::cout << "Minimum bounds: "
              << meshBounds.minimum().x() << ", "
              << meshBounds.minimum().y() << ", "
              << meshBounds.minimum().z() << '\n';

    std::cout << "Maximum bounds: "
              << meshBounds.maximum().x() << ", "
              << meshBounds.maximum().y() << ", "
              << meshBounds.maximum().z() << '\n';
}