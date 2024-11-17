#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

#include "spdlog/spdlog.h"
//
#include "core.h"

// pinhole camera
class Camera {
private:
    Vec3f position;
    Vec3f forward;
    Vec3f right;
    Vec3f up;

    float FOV;
    float focal_length;

public:
    Camera(const Vec3f& position, const Vec3f& forward, float FOV = 0.5f * PI);

    // sample ray emitting from the given sensor coordinate
    // NOTE: uv: [-1, -1] x [1, 1], sensor coordinate
    bool sampleRay(const Vec2f& uv, Ray& ray, float& pdf) const;
};

#endif //CAMERA_H
