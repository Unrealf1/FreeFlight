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
    Test,
};

using biomeId_t = BiomeId;

class Biome {
public:
    Biome() = default;
    virtual ~Biome() = default;

    virtual void generateHeights(std::vector<std::vector<float>>& heights, const glm::vec2& far_left, float step) = 0;
    virtual GLuint getTexture() = 0;
    virtual biomeId_t getId() = 0;
    virtual std::string name() = 0;
protected:
};