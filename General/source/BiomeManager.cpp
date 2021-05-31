#include "biomes/BiomeManager.hpp"

#include <spdlog/spdlog.h>
#include <cmath>
#include "biomes/TestBiome.hpp"


void BiomeManager::generateHeights(
    std::vector<std::vector<float>>& heights, 
    const glm::vec2& far_left,
    float step) {    
    auto biome = TestBiome();
    biome.generateHeights(heights, far_left, step);
}