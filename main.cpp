#include <spdlog/spdlog.h>
#include <tiny_obj_loader.h>
#include <omp.h>
#include <iostream>

int main() {
    //just for testing included libraries
    tinyobj::attrib_t attrib;
    spdlog::info("Hello, world!");
#pragma omp parallel
    {
        const int thread_id = omp_get_thread_num();
        spdlog::info("Hello from thread " + std::to_string(thread_id));
    }
    return 0;
}
