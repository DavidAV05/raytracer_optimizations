cmake_minimum_required(VERSION 3.20)

project(simple_renderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(simple_renderer src/main.cpp
        src/TinyObjLoader.cpp
        src/Types/Vec3.h
        src/Types/Color.h
        src/Intersections/Ray.h
        src/Intersections/Hittable.h
        src/Intersections/Aabb.h
        Shapes/Sphere.h
        Shapes/Triangle.h
        src/Scene/ObjLoader.h
        third_party/tinyobjloader/tiny_obj_loader.h
        src/Intersections/HittableList.h
        src/Types/SimpleRT.h
        src/Types/Interval.h
        src/Rendering/Camera.h
        Materials/Material.h
        Materials/DiffuseMaterial.h
        lights/Pointlight.h
        Materials/PhongMaterial.h
        src/Scene/Scene.h
        Shapes/Plane.h
        Textures/CheckerBoardTexture.h
        Textures/Texture.h
        Textures/SolidColor.h
        Materials/DialecticMaterial.h
        src/Intersections/RayTracer.h
        src/Rendering/RenderStats.h
        src/Rendering/Renderer.h
        src/BVH-Emre/BVH.h
        src/BVH-Emre/BVHValidation.h
        src/BVH-Emre/BVHValidationMain.cpp
        src/BVH-Emre/BVHValidationMain.h
        src/BVH-Emre/TraversalStats.h
        src/BVH-Emre/BVHMeshBenchmark.h
        src/BVH-Emre/BVHCameraBenchmark.h)

set_property(TARGET simple_renderer PROPERTY
        VS_DEBUGGER_WORKING_DIRECTORY ".")

configure_file(objFiles/monkey.obj objFiles/monkey.obj COPYONLY)

if(MSVC)
    target_compile_options(simple_renderer PRIVATE /W4)
else()
    target_compile_options(simple_renderer PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror=unused-result
    )
endif()