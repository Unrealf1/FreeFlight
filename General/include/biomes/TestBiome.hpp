#pragma once

#include "biomes/Biome.hpp"
#include "biomes/simplexnoise1234.h"
#include "render/TextureContainer.hpp"

#include <spdlog/spdlog.h>
#include <fmt/core.h>


class TestBiome: public Biome {
public:
    TestBiome(const char* texname = "resources/textures/grass3.jpg") {
        glGenSamplers(1, &_sampler);
        glSamplerParameterf(_sampler, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glSamplerParameterf(_sampler, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        _texture = TextureContainer::getTexture(texname);
        _texHandle = TexHandleContainer::createHandle(_texture, _sampler);
        glMakeTextureHandleResidentARB(_texHandle);
    }

    void generateVertices(TerrainChunk::vertexMap_t& vertices, const glm::vec2& near_left, float step) override {
        std::vector<std::vector<float>> heights;
        heights.resize(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i) {
            heights[i].resize(vertices[i].size());
        }
        generateHeightsLinear(heights, near_left, step);
        
        for (size_t i = 0; i < vertices.size(); ++i) {
            for (size_t j = 0; j < vertices[i].size(); ++j) {
                vertices[i][j].height = heights[i][j];
                vertices[i][j].texture_handler = _texHandle;
            }
        }
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
    GLuint _sampler;
    GLuint _texture;
    GLuint64 _texHandle;


    void generateHeightsNoise(std::vector<std::vector<float>>& heights, const glm::vec2& near_left, float step) {
        float y = near_left.y;
        for (auto& row : heights) {
            float x = near_left.x;
            for (auto& elem : row) {
                elem = SimplexNoise1234::noise(0.0008f * x, 0.0008f * y) * 10.0f;
                //elem += std::sin(0.001f * x) / 2.0f; 
                elem += SimplexNoise1234::noise(0.008f * (x + 777.0f), 0.008f * (y + 12345.0f)) * 5.0f;;
                x += step;
            }
            y += step;
        }
    }

    void generateHeightsFlat(std::vector<std::vector<float>>& heights, const glm::vec2&, float) {
        for (auto& row : heights) {
            for (auto& elem : row) {
                elem = 0.0f;
            }
        }
    }

    void generateHeightsLinear(std::vector<std::vector<float>>& heights, const glm::vec2& near_left, float step) {
        float y = near_left.y;
        float alpha = 0.1f;
        for (auto& row : heights) {
            float x = near_left.x;
            for (auto& elem : row) {
                elem = (y) * alpha;
                fmt::print("{} ({}, {}), ", elem, x, y);
                x += step;
            }
            y += step;
            fmt::print("\n");
        }
        spdlog::debug("near left is {} {}", near_left.x, near_left.y);
        spdlog::debug("step is {}, size is {}", step, heights.size());
    }

    void generateHeightsSin(std::vector<std::vector<float>>& heights, const glm::vec2& near_left, float step) {
        float y = near_left.y;
        for (auto& row : heights) {
            float x = near_left.x;
            for (auto& elem : row) {
                elem = std::sin(x) + std::cos(y * 0.001f);
                x += step;
            }
            y += step;
        }
    }

};