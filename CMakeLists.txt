cmake_minimum_required(VERSION 3.20)

project(simple_renderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(simple_renderer src/main.cpp
        src/TinyObjLoader.cpp
        src/Vec3.h
        src/Color.h
        src/Ray.h
        src/Hittable.h
        src/Aabb.h
        Shapes/Sphere.h
        Shapes/Triangle.h
        src/ObjLoader.h
        third_party/tinyobjloader/tiny_obj_loader.h
        src/HittableList.h
        src/SimpleRT.h
        src/Interval.h
        src/Camera.h
        Materials/Material.h
        Materials/DiffuseMaterial.h
        lights/Pointlight.h
        Materials/PhongMaterial.h
        src/Scene.h
        Shapes/Plane.h
        Textures/CheckerBoardTexture.h
        Textures/Texture.h
        Textures/SolidColor.h
        Materials/DialecticMaterial.h
        src/RayTracer.h
        src/RenderStats.h
        src/Renderer.h)

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