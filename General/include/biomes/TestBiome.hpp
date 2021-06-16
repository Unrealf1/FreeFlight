#pragma once

#include "biomes/Biome.hpp"
#include "biomes/simplexnoise1234.h"
#include <spdlog/spdlog.h>
#include <fmt/core.h>


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

    void generateHeightsLinear(std::vector<std::vector<float>>& heights, const glm::vec2& far_left, float step) {
        float y = far_left.y;
        float alpha = 0.01f;
        for (auto& row : heights) {
            float x = far_left.x;
            for (auto& elem : row) {
                elem = (y) * alpha;
                fmt::print("{} ({}, {}), ", elem, x, y);
                x += step;
            }
            y -= step;
            fmt::print("\n");
        }
        spdlog::error("far left is {} {}", far_left.x, far_left.y);
        spdlog::error("step is {}, size is {}", step, heights.size());
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