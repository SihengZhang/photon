#ifndef _IMAGE_H
#define _IMAGE_H
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "core.h"

class Image {
 private:
  unsigned int width;
  unsigned int height;
  std::vector<float> pixels;

 private:
  [[nodiscard]] unsigned int getIndex(unsigned int i, unsigned int j) const {
    return 3 * j + 3 * width * i;
  }

 public:
  Image(unsigned int width, unsigned int height);

  [[nodiscard]] Vec3f getPixel(unsigned int i, unsigned int j) const;

  void addPixel(unsigned int i, unsigned int j, const Vec3f& rgb);

  void setPixel(unsigned int i, unsigned int j, const Vec3f& rgb);

  void divide(float k);

  void gammaCorrection(float gamma);

  void writePPM(const std::string& filename) const;
};

#endif