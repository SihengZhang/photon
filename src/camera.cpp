#include "camera.h"

Camera::Camera(const Vec3f& position, const Vec3f& forward, const float FOV)
        : position(position), forward(forward), FOV(FOV) {
    right = normalize(cross(forward, Vec3f(0, 1, 0)));
    up = normalize(cross(right, forward));

    spdlog::info("[Camera] position: ({}, {}, {})", position[0], position[1],
                 position[2]);
    spdlog::info("[Camera] forward: ({}, {}, {})", forward[0], forward[1],
                 forward[2]);
    spdlog::info("[Camera] right: ({}, {}, {})", right[0], right[1], right[2]);
    spdlog::info("[Camera] up: ({}, {}, {})", up[0], up[1], up[2]);

    // compute focal length from FOV
    focal_length = 1.0f / std::tan(0.5f * FOV);

    spdlog::info("[Camera] focal_length: {}", focal_length);
}

bool Camera::sampleRay(const Vec2f& uv, Ray& ray, float& pdf) const {
    const Vec3f pinholePos = position + focal_length * forward;
    const Vec3f sensorPos = position + uv[0] * right + uv[1] * up;
    ray = Ray(sensorPos, normalize(pinholePos - sensorPos));
    pdf = 1.0f;
    return true;
}
