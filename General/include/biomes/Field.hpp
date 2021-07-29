#pragma once

#include "biomes/Biome.hpp"
#include "biomes/simplexnoise1234.h"
#include "render/TextureContainer.hpp"

#include <spdlog/spdlog.h>
#include <fmt/core.h>

class Field: public Biome {
public:
    Field(const char* texname = "resources/textures/grass3.jpg") {
        glGenSamplers(1, &_sampler);
        glSamplerParameterf(_sampler, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glSamplerParameterf(_sampler, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        _texture = TextureContainer::getTexture(texname);
        _texHandle = TexHandleContainer::createHandle(_texture, _sampler);
        glMakeTextureHandleResidentARB(_texHandle);
    }

    void generateVertices(TerrainChunk::vertexMap_t& vertices, const glm::vec2& near_left, float step) override {
        float y = near_left.y;       
        for (size_t i = 0; i < vertices.size(); ++i) {
            float x = near_left.x;
            for (size_t j = 0; j < vertices[i].size(); ++j) {
                float height = 1.0f;
                height += SimplexNoise1234::noise(0.0008f * x, 0.0008f * y) * 8.0f;
                height += SimplexNoise1234::noise(0.003f * (x + 333.0f), 0.003f * (y + 54321.0f)) * 2.0f;
                height += (std::sin(0.00005f * x) + std::cos(0.00005f * y)) * 2.0f;

                vertices[i][j].height = height;
                vertices[i][j].texture_handler = _texHandle;
                x += step;
            }
            y += step;
        }
    }

    GLuint getTexture() {
        return 0;
    }
    biomeId_t getId() {
        return BiomeId::Test;
    }

    std::string name() {
        return "field";
    }
private:
    GLuint _sampler;
    GLuint _texture;
    GLuint64 _texHandle;
};
