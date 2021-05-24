#include "biomes/BiomeManager.hpp"

void BiomeManager::generateHeights(
    std::vector<std::vector<float>>& heights, 
    const glm::vec2& low_left) {
    
    for (auto& row : heights) {
        for (auto& elem : row) {
            elem = 0.0f;
        }
    }
}