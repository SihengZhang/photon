#include "camera.h"
#include "image.h"
#include "integrator.h"
#include "photon_map.h"
#include "scene.h"
#include "core.h"
#include "json.h"

#include <opencv2/opencv.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr<<"Usage: "<<argv[0]<<" <path_to_json_file>"<<std::endl;
        return 1;
    }

    std::filesystem::path json_path = argv[1];
    std::ifstream file(json_path);

    if (!file) {
        spdlog::error("Failed to open file {}", json_path.generic_string());
        return 1;
    }

    nlohmann::json json_data;

    try {
        file >> json_data;
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("Error parsing json {}", e.what());
        return 1;
    }

    int width;
    int height;
    int n_samples;
    int n_photons;
    int n_estimation_global;
    float n_photons_caustics_multiplier;
    int n_estimation_caustics;
    int final_gathering_depth;
    int max_depth;

    Vec3f position;
    Vec3f forward;
    Vec3f upward;
    float FOV;

    std::filesystem::path model_path;
    std::filesystem::path search_dir;
    std::filesystem::path output_dir;

    try {
        width = json_data.at("width").get<int>();
        height = json_data.at("height").get<int>();
        n_samples = json_data.at("n_samples").get<int>();
        n_photons = json_data.at("n_photons").get<int>();
        n_estimation_global = json_data.at("n_estimation_global").get<int>();
        n_photons_caustics_multiplier = json_data.at("n_photons_caustics_multiplier").get<float>();
        n_estimation_caustics = json_data.at("n_estimation_caustics").get<int>();
        final_gathering_depth = json_data.at("final_gathering_depth").get<int>();
        max_depth = json_data.at("max_depth").get<int>();

        const auto& camera_data = json_data.at("camera");
        position = Vec3f(camera_data["position"].get<std::vector<float>>());
        forward = Vec3f(camera_data["forward"].get<std::vector<float>>());
        upward = Vec3f(camera_data["upward"].get<std::vector<float>>());
        FOV = camera_data["FOV"].get<float>();

        model_path = json_data.at("model_path").get<std::string>();
        search_dir = json_data.at("search_dir").get<std::string>();
        output_dir = json_data.at("output_dir").get<std::string>();
    } catch (const nlohmann::json::out_of_range& e) {
        spdlog::error("Error accessing json key: {}", e.what());
        return 1;
    }

    Image image(width, height);
    Camera camera(position, forward, upward, FOV);

    Scene scene;
    spdlog::info("[main] Current working directory: {}", std::filesystem::current_path().generic_string());
    scene.loadModel(model_path, search_dir);
    scene.build();

    // photon tracing and build photon map
    PhotonMapping integrator(n_photons, n_estimation_global,
                           n_photons_caustics_multiplier, n_estimation_caustics,
                           final_gathering_depth, max_depth);
    UniformSampler sampler;
    integrator.build(scene, sampler);

    spdlog::info("[main] tracing rays from camera");


    int total_pixels = width * height;
    std::atomic<int> progress(0);
    std::atomic<bool> rendering_done(false);

    // Logging thread
    std::thread logger([&]() {
        while (!rendering_done) {
            spdlog::info("[main] Progress: {}/{} pixels completed", progress.load(), total_pixels);
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });
#pragma omp parallel
    {
        #pragma omp for collapse(2) schedule(dynamic, 1)
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                // init sampler
                UniformSampler sampler(j + width * i);

                for (int k = 0; k < n_samples; ++k) {
                    const float u = (2.0f * (static_cast<float>(j) + sampler.getNext1D()) - static_cast<float>(width)) / static_cast<float>(width);
                    const float v = (2.0f * (static_cast<float>(i) + sampler.getNext1D()) - static_cast<float>(height)) / static_cast<float>(height);

                    Ray ray;
                    float pdf;

                    if (camera.sampleRay(Vec2f(u, v), ray, pdf)) {

                        const Vec3f radiance = integrator.integrate(ray, scene, sampler) / pdf;

                        if (std::isnan(radiance[0]) || std::isnan(radiance[1]) || std::isnan(radiance[2])) {
                            spdlog::error("radiance is NaN");
                            continue;
                        }

                        if (radiance[0] < 0 || radiance[1] < 0 || radiance[2] < 0) {
                            spdlog::error("radiance is minus");
                            continue;
                        }

                        image.addPixel(i, j, radiance);
                    } else {
                        image.setPixel(i, j, Vec3f(0));
                    }
                }
                ++progress;
            }
        }
    }

    rendering_done = true;
    logger.join(); // Wait for the logger thread to finish

    // Take average per pixel
    image.divide(static_cast<float>(n_samples));

    image.gammaCorrection(2.2f);
    image.writePPM(output_dir.generic_string() + "output.ppm");

    // Convert .ppm file to .png file
    cv::Mat pixel_mat = cv::imread(output_dir.generic_string() + "output.ppm");
    cv::imwrite(output_dir.generic_string() + "output.png", pixel_mat);

    // Create a window and show converted image
    cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Image", pixel_mat);
    spdlog::info("[main] PRESS ANY KEY TO CLOSE THE PREVIEW WINDOW");
    cv::waitKey(0);

    return 0;
}
