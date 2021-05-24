#pragma once

#include "biomes/Biome.hpp"

class TestBiome : public Biome {
public:
    virtual void generateHeights(std::vector<std::vector<float>>& heights, const glm::vec2& low_left) {
        for (auto& row : heights) {
            for (auto& elem : row) {
                elem = 0;
            }
        }
    }
    virtual GLuint getTexture() {
        return 0;
    }
    virtual biomeId_t getId() {
        return 0;
    }
};