#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include "core.h"

class Image {

public:
    Image(unsigned int width, unsigned int height);

    [[nodiscard]] unsigned int getIndex(unsigned int i, unsigned int j) const;

    [[nodiscard]] Vec3f getPixel(unsigned int i, unsigned int j) const;

    void addPixel(unsigned int i, unsigned int j, const Vec3f& rgb);

    void setPixel(unsigned int i, unsigned int j, const Vec3f& rgb);

    void divide(float k);

    void gammaCorrection(float gamma);

    void writePPM(const std::string& filename) const;

private:

    unsigned int width;
    unsigned int height;
    std::vector<float> pixels;
};

#endif