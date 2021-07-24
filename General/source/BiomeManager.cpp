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

    for (uint32_t i = 1; i < result._vertices.size(); ++i) {
        for (uint32_t j = 1; j < result._vertices.size(); ++j) {
            ChunkVertex& vertex = result._vertices[i][j];

            if (result._vertices[i-1][j].texture_handler != vertex.texture_handler) {
                vertex.secondary_texture_handler = result._vertices[i-1][j].texture_handler;
                vertex.secondary_texture_weight = 1.0f;
            } else if(result._vertices[i][j-1].texture_handler != vertex.texture_handler) {
                vertex.secondary_texture_handler = result._vertices[i-1][j].texture_handler;
                vertex.secondary_texture_weight = 1.0f;
            } else {
                vertex.secondary_texture_handler = vertex.texture_handler;
                vertex.secondary_texture_weight = 0.0f;
            }
        }
    }   

    return result;
}


