#pragma once

#include "biomes/Biome.hpp"
#include "biomes/simplexnoise1234.h"
#include "render/TextureContainer.hpp"

#include <spdlog/spdlog.h>
#include <fmt/core.h>

class Desert: public SimpleBiome {
public:
    Desert(const char* texname = "resources/textures/minecraft_textures/block/sand.png") {
        glGenSamplers(1, &_sampler);
        glSamplerParameterf(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameterf(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

        _texture = TextureContainer::getTexture(texname);
        _texHandle = TexHandleContainer::createHandle(_texture, _sampler);
        glMakeTextureHandleResidentARB(_texHandle);
    }

    GLuint getTexture() {
        return 0;
    }
    biomeId_t getId() {
        return BiomeId::Desert;
    }

    std::string name() {
        return "desert";
    }

protected:
    ChunkVertex generateVertex(const glm::vec2& position, const BiomeCenter& related_center) {
        float x = position.x;
        float y = position.y;

        ChunkVertex res;
        float height = 20.0f;
        height += SimplexNoise1234::noise(0.0008f * x, 0.0008f * y) * 8.0f;
        height += SimplexNoise1234::noise(0.0045f * (x + 333.0f), 0.0015f * (y + 54321.0f)) * 20.0f;
        height += (std::sin(0.00005f * x) + std::cos(0.00005f * y)) * 5.0f;

        res.height = height;
        res.texture_handler = _texHandle;
        return res;
    }


private:
    GLuint _sampler;
    GLuint _texture;
    GLuint64 _texHandle;
};
