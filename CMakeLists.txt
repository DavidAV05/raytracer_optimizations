cmake_minimum_required(VERSION 3.20)

project(simple_renderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(simple_renderer main.cpp
        TinyObjLoader.cpp
        Vec3.h
        Color.h
        Ray.h
        Hittable.h
        Aabb.h
        Shapes/Sphere.h
        Shapes/Triangle.h
        ObjLoader.h
        third_party/tinyobjloader/tiny_obj_loader.h
        HittableList.h
        SimpleRT.h
        Interval.h
        Camera.h
        Materials/Material.h
        Materials/DiffuseMaterial.h
        lights/Pointlight.h
        Materials/PhongMaterial.h
        Scene.h
        Shapes/Plane.h
        Textures/CheckerBoardTexture.h
        Textures/Texture.h
        Textures/SolidColor.h
        Materials/DialecticMaterial.h
        RayTracer.h
        RenderStats.h
        Renderer.h)

set_property(TARGET simple_renderer PROPERTY
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

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