#include "biomes/BiomeManager.hpp"

#include <spdlog/spdlog.h>
#include <cmath>

void BiomeManager::generateHeights(
    std::vector<std::vector<float>>& heights, 
    const glm::vec2& far_left,
    float step) {
    spdlog::info("BiomeManager: generating heights near {} {}. Heights size is {}", far_left.x, far_left.y, heights.size());
    
    float y = far_left.y;
    for (auto& row : heights) {
        float x = far_left.x;
        y -= step;
        for (auto& elem : row) {
            //elem = 1.0f * x;
            elem = std::sin(0.1f * x); //+ std::cos(0.1f * y);
            elem *= 2.0f;
            //elem = (std::sin(0.01f * x) + std::cos(0.01f * y)) / 2.0f;
            x += step;
            
            //elem = 0.0f;
        }
    }
    spdlog::error("BiomeManager: final y is {}", y);
}