
#pragma once

#include "../src/Types/Color.h"
#include "../src/Types/Vec3.h"

struct PointLight {
    Point3 position;
    Color color{1.0, 1.0, 1.0};
    double intensity{1.0};
};
