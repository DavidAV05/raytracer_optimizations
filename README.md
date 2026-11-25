# Simple Renderer

## Building

This project uses CMake and C++20. TinyObjLoader is included in
`third_party/tinyobjloader`, so building does not require downloading a
dependency. Open the root `CMakeLists.txt` in Visual Studio, or configure and
build with CMake from another supported C++20 toolchain.

## Running

Run `simple_renderer` from the project directory to load `monkey.obj` beside
the floor. To load another OBJ instead, pass its path:

```text
simple_renderer path/to/mesh.obj
```

The OBJ is rendered using one mesh material. OBJ materials and textures are
not applied. Coordinates are converted from Z-up to this renderer's Y-up
coordinate system. The program overwrites
`render.ppm` and `render_stats.json` in its working directory. The JSON file
contains the total rendering time in seconds, the number of scene objects, and
the number of triangle objects.

TinyObjLoader is MIT-licensed. Its source and license are in
`third_party/tinyobjloader`; see that directory for the pinned upstream commit.
