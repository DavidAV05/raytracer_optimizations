#include <memory>
#include <iostream>
#include <string>

#include "Rendering/Camera.h"
#include "Scene/ObjLoader.h"
#include "Rendering/Renderer.h"
#include "../Materials/PhongMaterial.h"
#include "Scene/Scene.h"
#include "../Materials/DialecticMaterial.h"
#include "../Shapes/Plane.h"
#include "../Shapes/Triangle.h"
#include "../Textures/CheckerBoardTexture.h"
#include "../Textures/SolidColor.h"

int main(const int argc, char* argv[])
{
    if (argc > 2)
    {
        std::cerr << "Usage: simple_renderer [mesh.obj]\n";
        return 1;
    }

    const std::string meshPath = argc > 1 ? argv[1] : "./objFiles/monkey.obj";

    const auto checkerTexture = std::make_shared<CheckerTexture>( 1.5, Color{0.72, 0.72, 0.72}, Color{0.38, 0.38, 0.38});
    const auto groundMaterial = std::make_shared<PhongMaterial>( checkerTexture, 0.0, 32, 0.0, Color{1, 1, 1} );

    const auto meshTexture = std::make_shared<SolidColor>(Color{0.9, 0.45, 0.16});
    const auto meshMaterial = std::make_shared<PhongMaterial>(meshTexture, 0.0, 32, 0.0, Color{1, 1, 1} );

    Scene scene;

    addObjToScene(scene, meshPath, meshMaterial,Point3{2.0, 0.0, 0.0}, false);

    scene.addObject(std::make_shared<Plane>( Point3{0.0, -0.851562, 0.0}, Vec3{0.0, 1.0, 0.0}, groundMaterial));
    scene.addLight(PointLight{Point3{-4.0, 5.0, -3.0}, Color{1.0, 1.0, 1.0}, 0.8});

    scene.setAmbientLight(Color{0.16, 0.16, 0.16});

    Camera camera;
    camera.aspectRatio = 16.0 / 9.0;
    camera.imageWidth = 1000;
    camera.verticalFov = 40.0;

    camera.lookFrom = Point3{0.0, 6, 5.0};
    camera.lookAt = Point3{0.0, 0.0, 0.0};

    Renderer renderer;
    renderer.samplesPerPixel = 1;
    renderer.depth = 3;
    renderer.shadowsEnabled = true;
    renderer.render(camera, scene);

    return 0;
}
