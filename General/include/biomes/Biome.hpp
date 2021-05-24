#pragma once

#include "Typedefs.hpp"
#include "terrain/TerrainChunk.hpp"

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>


class Biome {
public:
    Biome() = default;
    virtual ~Biome() = default;

    virtual void generateHeights(std::vector<std::vector<float>>& heights, const glm::vec2& low_left) = 0;
    virtual GLuint getTexture() = 0;
    virtual biomeId_t getId() = 0;
    virtual std::string name() = 0;

    static std::unordered_map<biomeId_t, std::unique_ptr<Biome>> all_biomes;
protected:
    static void registerBiome(std::unique_ptr<Biome> biome) {
        all_biomes.insert(std::make_pair(biome->getId(), std::move(biome)));
    }
};