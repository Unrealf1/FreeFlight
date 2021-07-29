#include "biomes/BiomeManager.hpp"
#include "biomes/TestBiome.hpp"
#include "biomes/Hills.hpp"
#include "biomes/Field.hpp"

#include <spdlog/spdlog.h>
#include <cmath>
#include <cstdlib>


TerrainChunk BiomeManager::generateChunk(uint32_t points_in_chunk, const glm::vec2& near_left, float step) {
    TerrainChunk result;
    result._vertices.resize(points_in_chunk);
    for (auto& row : result._vertices) {
        row.resize(points_in_chunk);
    }

    TerrainChunk::vertexMap_t first = result._vertices;
    TerrainChunk::vertexMap_t second = result._vertices;
    Hills().generateVertices(first, near_left, step);
    Field().generateVertices(second, near_left, step);

    for (uint32_t i = 0; i < result._vertices.size(); ++i) {
        for (uint32_t j = 0; j < result._vertices.size(); ++j) {
            auto x = near_left.x + static_cast<float>(j) * step;
            auto y = near_left.y + static_cast<float>(i) * step;

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
            //result._vertices[i][j] = first[i][j];

            if (std::abs(x - 100.0f) < 100.0f && std::abs(y - 10.0f) < 100.0f) {
                result._vertices[i][j].height = -50.0f;
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
    for (uint32_t j = 1; j < result._vertices.size(); ++j) {
        result._vertices[0][j].secondary_texture_weight = 0.0f;
        result._vertices[0][j].secondary_texture_handler = result._vertices[0][j].texture_handler;
        result._vertices[j][0].secondary_texture_weight = 0.0f;
        result._vertices[j][0].secondary_texture_handler = result._vertices[j][0].texture_handler;
    }


    return result;
}


