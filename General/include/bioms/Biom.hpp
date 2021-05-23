#pragma once

#include "Typedefs.hpp"
#include "terrain/TerrainChunk.hpp"

#include <glm/glm.h>

class Biom {
public:
    virtual void generateHeights(std::vector<std::vector<float>>& heights, const glm::vec2& low_left) = 0;
    virtual GLuint getTexture() = 0;
    virtual biomId_t getId() = 0;
};