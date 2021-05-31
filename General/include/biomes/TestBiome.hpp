#pragma once

#include "biomes/Biome.hpp"
#include "biomes/simplexnoise1234.h"


class TestBiome: public Biome {
public:
    void generateHeights(std::vector<std::vector<float>>& heights, const glm::vec2& far_left, float step) {
        generateHeightsNoise(heights, far_left, step);
    }

    GLuint getTexture() {
        return 0;
    }
    biomeId_t getId() {
        return BiomeId::Test;
    }

    std::string name() {
        return "test_biome";
    }
private:
    void generateHeightsNoise(std::vector<std::vector<float>>& heights, const glm::vec2& far_left, float step) {
        float y = far_left.y;
        for (auto& row : heights) {
            float x = far_left.x;
            y -= step;
            for (auto& elem : row) {
                elem = SimplexNoise1234::noise(0.0008f * x, 0.0008f * y);
                //elem += std::sin(0.001f * x) / 2.0f; 
                elem *= 10.0f;
                x += step;
            }
        }
    }

    void generateHeightsFlat(std::vector<std::vector<float>>& heights, const glm::vec2& far_left, float step) {
        for (auto& row : heights) {
            for (auto& elem : row) {
                elem = 0.0f;
            }
        }
    }

    void generateHeightsSin(std::vector<std::vector<float>>& heights, const glm::vec2& far_left, float step) {
        float y = far_left.y;
        for (auto& row : heights) {
            float x = far_left.x;
            y -= step;
            for (auto& elem : row) {
                elem = std::sin(x) + std::cos(y * 0.001f);
                x += step;
            }
        }
    }

};