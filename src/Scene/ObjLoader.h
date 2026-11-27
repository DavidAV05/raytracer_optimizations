#pragma once

#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../Materials/Material.h"
#include "Scene.h"
#include "../../Shapes/Triangle.h"
#include "../../third_party/tinyobjloader/tiny_obj_loader.h"

[[nodiscard]] inline std::vector<Triangle> loadWavefrontObj(
    const std::string& path,
    std::shared_ptr<const Material> material,
    const bool convertZUpToYUp = false,
    const Vec3& position = Vec3{}
)
{
    if (!material)
    {
        throw std::invalid_argument("OBJ material must not be null");
    }

    std::ifstream input(path);
    if (!input)
    {
        throw std::runtime_error("Could not open OBJ file: " + path);
    }

    std::ostringstream objText;
    std::string line;
    while (std::getline(input, line))
    {
        std::istringstream row(line);
        std::string directive;
        row >> directive;
        if (directive != "mtllib" && directive != "usemtl")
        {
            objText << line << '\n';
        }
    }
    if (input.bad())
    {
        throw std::runtime_error("Failed while reading OBJ file: " + path);
    }

    tinyobj::ObjReaderConfig config;
    config.triangulate = true;
    tinyobj::ObjReader reader;
    if (!reader.ParseFromString(objText.str(), "", config))
    {
        throw std::runtime_error("Could not load OBJ file '" + path + "': " + reader.Error());
    }

    const tinyobj::attrib_t& attributes = reader.GetAttrib();
    const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
    std::vector<Triangle> triangles;

    for (const tinyobj::shape_t& shape : shapes)
    {
        std::size_t indexOffset = 0;
        for (const unsigned int verticesInFace : shape.mesh.num_face_vertices)
        {
            if (verticesInFace != 3)
            {
                throw std::runtime_error("TinyObjLoader returned a non-triangle face for '" + path + "'");
            }
            if (indexOffset > shape.mesh.indices.size() ||
                verticesInFace > shape.mesh.indices.size() - indexOffset)
            {
                throw std::runtime_error("Invalid face indices in OBJ file '" + path + "'");
            }

            Point3 points[3];
            for (std::size_t corner = 0; corner < 3; ++corner)
            {
                const int vertexIndex = shape.mesh.indices[indexOffset + corner].vertex_index;
                if (vertexIndex < 0 || attributes.vertices.size() < 3 ||
                    static_cast<std::size_t>(vertexIndex) > (attributes.vertices.size() - 3) / 3)
                {
                    throw std::runtime_error("Invalid vertex index in OBJ file '" + path + "'");
                }

                const std::size_t offset = static_cast<std::size_t>(vertexIndex) * 3;
                const double x = attributes.vertices[offset];
                const double y = attributes.vertices[offset + 1];
                const double z = attributes.vertices[offset + 2];
                points[corner] = (convertZUpToYUp
                                      ? Point3{x, z, y}
                                      : Point3{x, y, z}) + position;
            }

            if (convertZUpToYUp)
            {
                triangles.emplace_back(points[0], points[2], points[1], material);
            }
            else
            {
                triangles.emplace_back(points[0], points[1], points[2], material);
            }
            indexOffset += verticesInFace;
        }
    }

    if (triangles.empty())
    {
        throw std::runtime_error("OBJ file contains no triangles: " + path);
    }
    return triangles;
}



/// loads an obj file and adds it to the given scene
inline void addObjToScene(
    Scene& scene,
    const std::string& path,
    const std::shared_ptr<const Material>& material,
    const Vec3& position = Vec3{0.0, 0.0, 0.0},
    const bool convertZUpToYUp = false
)
{
    for (Triangle& triangle : loadWavefrontObj(path, material, convertZUpToYUp, position))
    {
        scene.addObject(std::make_shared<Triangle>(std::move(triangle)));
    }
}
