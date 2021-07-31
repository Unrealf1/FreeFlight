#pragma once

#include "terrain/TerrainChunk.hpp"

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <cmath>


enum class BiomeId {
    Test, Field, Hills, Desert
};

using biomeId_t = BiomeId;

struct BiomeCenter {
    glm::vec2 position;
    biomeId_t biome_id;
};

class Biome {
public:
    Biome() = default;
    virtual ~Biome() = default;

    virtual void generateVertices(
        std::vector<std::vector<ChunkVertex>>& vertices, 
        const glm::vec2& near_left, 
        float step, 
        const std::vector<std::vector<BiomeCenter>>& related_centers) = 0;
    virtual GLuint getTexture() = 0;
    virtual biomeId_t getId() = 0;
    virtual std::string name() = 0;
protected:
};

class SimpleBiome : public Biome {
public:
    SimpleBiome() = default;
    virtual ~SimpleBiome() = default;

    virtual void generateVertices(
        std::vector<std::vector<ChunkVertex>>& vertices, 
        const glm::vec2& near_left, 
        float step, 
        const std::vector<std::vector<BiomeCenter>>& related_centers)
        {
            glm::vec2 current_pos = near_left;
            for (size_t i = 0; i < vertices.size(); ++i) {
                for (size_t j = 0; j < vertices[i].size(); ++j) {
                    if (related_centers[i][j].biome_id == getId()) {
                        vertices[i][j] = generateVertex(current_pos, related_centers[i][j]);
                    }
                    current_pos.x += step;
                }

                current_pos.x = near_left.x;
                current_pos.y += step;
            }
        }
protected:
    virtual ChunkVertex generateVertex(const glm::vec2& position, const BiomeCenter& related_center) = 0;
};
