#ifndef _SHAPE_H
#define _SHAPE_H
#include "core.h"
#include "sampler.h"

class Triangle {

public:
    Triangle(const float* vertices, const uint32_t* indices, const float* normals,
        const float* texcoords, uint32_t faceID);

    // return vertex position
    [[nodiscard]] Vec3f getVertexPosition(uint32_t vertexID) const;

    // return vertex normal
    [[nodiscard]] Vec3f getVertexNormal(uint32_t vertexID) const;

    // return vertex texcoords
    [[nodiscard]] Vec2f getVertexTexcoords(uint32_t vertexID) const;

    // return vertex indices
    [[nodiscard]] Vec3ui getIndices() const;

    // return geometric normal
    [[nodiscard]] Vec3f getGeometricNormal() const;

    // compute shading normal at given position
    [[nodiscard]] Vec3f computeShadingNormal(const Vec2f& barycentric) const;

    // compute texcoords at given position
    [[nodiscard]] Vec2f getTexcoords(const Vec2f& barycentric) const;

    // sample point on the triangle
    SurfaceInfo samplePoint(Sampler& sampler, float& pdf) const;

private:
    const float* vertices;
    const uint32_t* indices;
    const float* normals;
    const float* texcoords;

    const uint32_t faceID;
    Vec3f geometricNormal;
    float surfaceArea;
};

#endif