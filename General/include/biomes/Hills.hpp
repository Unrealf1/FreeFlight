#pragma once

#include "biomes/Biome.hpp"
#include "biomes/simplexnoise1234.h"
#include "render/TextureContainer.hpp"

#include <spdlog/spdlog.h>
#include <fmt/core.h>

class Hills: public SimpleBiome {
public:
    Hills() {
        const char* texname_bot = "resources/textures/minecraft_textures/block/mossy_cobblestone.png";
        const char* texname_top = "resources/textures/minecraft_textures/block/cobblestone.png";
        
        glGenSamplers(1, &_sampler);
        glSamplerParameterf(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameterf(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

        _texture = TextureContainer::getTexture(texname_bot);
        _texHandle = TexHandleContainer::createHandle(_texture, _sampler);

        auto tex2 = TextureContainer::getTexture(texname_top);
        _texHandle2 = TexHandleContainer::createHandle(tex2, _sampler);
        glMakeTextureHandleResidentARB(_texHandle);
        glMakeTextureHandleResidentARB(_texHandle2);
    }

    GLuint getTexture() {
        return 0;
    }
    biomeId_t getId() {
        return BiomeId::Hills;
    }

    std::string name() {
        return "hills";
    }

protected:
    ChunkVertex generateVertex(const glm::vec2& position, const BiomeCenter& related_center) {
        float x = position.x;
        float y = position.y;
        ChunkVertex res;

        float height = 60.0f;
        height += SimplexNoise1234::noise(0.0008f * x, 0.0008f * y) * 12.0f;
        float slowness = 0.001f;
        height += SimplexNoise1234::noise(slowness * (x + 333.0f), slowness * (y + 54321.0f)) * 130.0f;
        height += SimplexNoise1234::noise(slowness * 15.0f * (x + 73145.0f), slowness * 15.0f * (y + 400.0f)) * 17.0f;
        height += (std::sin(0.00005f * x) + std::cos(0.00005f * y) + 1.0f) * 10.0f;

        res.height = height;
        res.texture_handler = height > 100.0f ? _texHandle2 : _texHandle;
        return res;
    }

private:
    GLuint _sampler;
    GLuint _texture;
    GLuint64 _texHandle;
    GLuint64 _texHandle2;
};
