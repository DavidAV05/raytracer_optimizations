# Simple Renderer

a small witted style C++20 ray tracer i made, based on the concepts in
[Ray Tracing in One Weekend](https://raytracing.github.io/).

*Sidenote: This implementation differs from the book in a few  areas and does not use path tracing.*

## Features

- sphere, plane, and triangle geometry
- Wavefrnt obj mesh loading (into triangles) using [TinyObjLoader](https://github.com/tinyobjloader/tinyobjloader)
- diffuse, reflective, and dielectric materials
- Blinn-Phong direct lighting
- Point lights and ambient lighting
- hard shadows & transmission thru glass
- antialiasing using per-pixel samples

## Requirements

- C++20 compiler
- CMake >=3.20 

[TinyObjLoader](https://github.com/tinyobjloader/tinyobjloader) is included in
`third_party/tinyobjloader`

## Building

You can open the project root in CLion (or Visual Studio) and build the
`simple_renderer` target from the CMake proj file.

OR 
manually from the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

executable is placed at `build/simple_renderer` on Linux or
`build/Release/simple_renderer.exe` on a single-config Visual Studio
setup.



## Outputs

The executable writes:

- `render.ppm` : the rendered image in PPM format
- `render_stats.json` : render time, object count, and triangle count

## Project layout

- `main.cpp` : example scene and renderer configuration
- `Camera.h`, `Ray.h`, `RayTracer.h` : camera and ray tracing
- `Shapes/` : sphere, plane, and triangle primitives
- `Materials/` : diffuse, reflective, and dielectric materials
- `Textures/` : material textures
- `lights/` : point-light implementation
- `ObjLoader.h` : TinyObjLoader-based OBJ support
- `Renderer.h` : image and render-stat output
