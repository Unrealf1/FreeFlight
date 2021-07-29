#pragma once

#include "biomes/Biome.hpp"

#include <vector>
#include <memory>
#include <glm/glm.hpp>


class BiomeManager {
public:
    BiomeManager() = default;
    virtual ~BiomeManager() = default;

    std::shared_ptr<Biome> get_biome_at(const glm::vec3& position);
    virtual TerrainChunk generateChunk(uint32_t points_in_chunk, const glm::vec2& near_left, float step);
private:
    std::vector<std::shared_ptr<Biome>> _active_biomes;

};