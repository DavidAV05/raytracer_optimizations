#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>


#include "Camera.h"
#include "../Intersections/RayTracer.h"
#include "../RenderStats.h"
#include "../Scene.h"

class Renderer final
{
public:
    int samplesPerPixel = 5;
    int depth = 1;
    bool shadowsEnabled = true;

    void render(Camera& camera, const Scene& scene) const
    {
        if (samplesPerPixel <= 0)
        {
            throw std::invalid_argument("Samples per pixel must be positive");
        }

        camera.initialize();

        const RayTracer rayTracer;
        const double sampleScale = 1.0 / samplesPerPixel;

        std::ofstream image{"render.ppm"};

        if (!image)
        {
            throw std::runtime_error("Could not create render.ppm");
        }

        image << "P3\n"
            << camera.imageWidth << ' ' << camera.imageHeight()
            << "\n255\n";

        const auto renderStart = std::chrono::steady_clock::now();
        for (int y = 0; y < camera.imageHeight(); ++y)
        {
            if (y % std::max(1, camera.imageHeight() / 10) == 0)
            {
                std::clog << "Rendering: "
                    << 100 * y / camera.imageHeight()
                    << "%\n";
            }

            for (int x = 0; x < camera.imageWidth; ++x)
            {
                Color pixelColor{0.0, 0.0, 0.0};

                for (int sample = 0; sample < samplesPerPixel; ++sample)
                {
                    const bool jitterForAntiAliasing = samplesPerPixel > 1;
                    const Ray ray = camera.getPrimaryRay(x, y, jitterForAntiAliasing );
                    pixelColor = pixelColor + rayTracer.trace(ray, scene, depth, shadowsEnabled);
                }

                writeColor(image, sampleScale * pixelColor);
            }
        }
        if (!image)
        {
            throw std::runtime_error("Failed while writing render.ppm");
        }

        const std::chrono::duration<double> renderTime = std::chrono::steady_clock::now() - renderStart;

        std::ofstream stats{"render_stats.json"};

        if (!stats)
        {
            throw std::runtime_error("Could not create render_stats.json");
        }
        // render stats in render_stats.json
        RenderStats{
            .totalRenderingTimeSeconds = renderTime.count(),
            .objectCount = scene.objectCount(),
            .triangleCount = scene.triangleCount(),
        }.writeJson(stats);

        std::clog << "Rendering: 100%\n";
    }
};
