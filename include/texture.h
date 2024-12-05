#ifndef TEXTURE_H
#define TEXTURE_H
#include <vector>
#include <string>
#include "core.h"

#include <opencv2/opencv.hpp>

class Texture {
private:
    std::vector<Vec3f> data;
    int width{0}, height{0};
    std::string filepath;
    bool loadingAttempted{false};
    Vec3f fallbackColor{1.0f};

public:
    Texture(const std::string& _filepath) : filepath(_filepath) {
        loadingAttempted = true;

        // Load the image using OpenCV
        cv::Mat img = cv::imread(filepath, cv::IMREAD_UNCHANGED);
        if (img.empty()) {
            // Print error only once during construction
            std::cerr << "Failed to load texture: " << filepath << "\n"
                      << "Will use fallback color instead." << std::endl;
            return;
        }

        // Convert to floating point, normalize amd switch to RGB channel
        img.convertTo(img, CV_32F, 1.0 / 255.0);
        cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

        // Store image dimensions
        width = img.cols;
        height = img.rows;

        try {
            data.resize(width * height);

            if (img.channels() == 3 || img.channels() == 4) {
                // Process RGB(A) image
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        Vec3f color = img.at<Vec3f>(y, x);
                        // Apply gamma correction
                        color[0] = std::pow(color[0], 2.2f);
                        color[1] = std::pow(color[1], 2.2f);
                        color[2] = std::pow(color[2], 2.2f);
                        data[y * width + x] = color;
                    }
                }
            } else {
                // Unsupported channel count
                throw std::runtime_error("Unsupported channel count: " + std::to_string(img.channels()));
            }

            std::cout << "Successfully loaded texture: " << filepath << " ("
                      << width << "x" << height << ")" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error processing texture data: " << e.what() << std::endl;
            data.clear();
            width = height = 0;
        }
    }

    Vec3f sample(const Vec2f& uv) const {
        if (data.empty()) {

            return fallbackColor;
        }

        float u = uv[0] - std::floor(uv[0]);
        float v = 1.0f - (uv[1] - std::floor(uv[1]));

        float fx = u * (width - 1);
        float fy = v * (height - 1);

        int x0 = std::max(0, std::min(static_cast<int>(fx), width - 1));
        int y0 = std::max(0, std::min(static_cast<int>(fy), height - 1));
        int x1 = std::min(x0 + 1, width - 1);
        int y1 = std::min(y0 + 1, height - 1);

        float dx = fx - x0;
        float dy = fy - y0;

        Vec3f c00 = data[y0 * width + x0];
        Vec3f c10 = data[y0 * width + x1];
        Vec3f c01 = data[y1 * width + x0];
        Vec3f c11 = data[y1 * width + x1];

        Vec3f c0 = c00 * (1 - dx) + c10 * dx;
        Vec3f c1 = c01 * (1 - dx) + c11 * dx;
        return c0 * (1 - dy) + c1 * dy;
    }

    void setFallbackColor(const Vec3f& color) {
        fallbackColor = color;
    }

    bool isValid() const {
        return !data.empty() && width > 0 && height > 0;
    }
};
#endif //TEXTURE_H
