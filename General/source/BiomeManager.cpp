#include "biomes/BiomeManager.hpp"
#include "biomes/TestBiome.hpp"

#include <spdlog/spdlog.h>
#include <cmath>
#include <cstdlib>


TerrainChunk BiomeManager::generateChunk(uint32_t points_in_chunk, const glm::vec2& far_left, float step) {
    TerrainChunk result;
    result._vertices.resize(points_in_chunk);
    for (auto& row : result._vertices) {
        row.resize(points_in_chunk);
    }
    std::vector<std::string> possible_textures = {
        "resources/textures/grass2.jpg", 
        "resources/textures/grass3.jpg", 
        "resources/textures/grass4.jpg", 
        "resources/textures/grass5.jpg", 
        "resources/textures/ground1.jpg",
        "resources/textures/sand1.jpg", 
        "resources/textures/sand2.jpg", 
        "resources/textures/sand3.jpg", 
        "resources/textures/sand4.jpg"
    };
    size_t index = rand()%possible_textures.size();

    auto biome = TestBiome(possible_textures[index].c_str());
    biome.generateVertices(result._vertices, far_left, step);

    return result;
}
