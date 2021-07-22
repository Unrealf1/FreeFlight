#include "biomes/BiomeManager.hpp"
#include "biomes/TestBiome.hpp"
#include "biomes/Hills.hpp"
#include "biomes/Field.hpp"

#include <spdlog/spdlog.h>
#include <cmath>
#include <cstdlib>


TerrainChunk BiomeManager::generateChunk(uint32_t points_in_chunk, const glm::vec2& far_left, float step) {
    TerrainChunk result;
    result._vertices.resize(points_in_chunk);
    for (auto& row : result._vertices) {
        row.resize(points_in_chunk);
    }

    TerrainChunk::vertexMap_t first = result._vertices;
    TerrainChunk::vertexMap_t second = result._vertices;
    Hills().generateVertices(first, far_left, step);
    Field().generateVertices(second, far_left, step);

    for (uint32_t i = 0; i < result._vertices.size(); ++i) {
        for (uint32_t j = 0; j < result._vertices.size(); ++j) {
            auto x = far_left.x + j * step;
            auto y = far_left.y - i * step;

            float repeat_k = 6000.0f;
            float radius = 600.0f; 

            auto k2x = std::round(x / repeat_k);
            auto k2y = std::round(y / repeat_k);

            glm::vec2 center{k2x * repeat_k, k2y * repeat_k};
            
            if (glm::distance(glm::vec2(x, y*1.5f), center) < radius) {
                result._vertices[i][j] = first[i][j];
            } else {
                result._vertices[i][j] = second[i][j];
            }
        }
    }

    // std::vector<std::string> possible_textures = {
    //     "resources/textures/grass2.jpg", 
    //     "resources/textures/grass3.jpg", 
    //     "resources/textures/grass4.jpg", 
    //     "resources/textures/grass5.jpg", 
    //     "resources/textures/ground1.jpg",
    //     "resources/textures/sand1.jpg", 
    //     "resources/textures/sand2.jpg", 
    //     "resources/textures/sand3.jpg", 
    //     "resources/textures/sand4.jpg"
    // };
    // size_t index = rand()%possible_textures.size();

    // auto biome = TestBiome(possible_textures[index].c_str());
    // generateVertices(result._vertices, far_left, step);

    //rand()%2 ? Hills().generateVertices(result._vertices, far_left, step) : Field().generateVertices(result._vertices, far_left, step);
    

    return result;
}


