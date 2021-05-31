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
    virtual void generateHeights(std::vector<std::vector<float>>& heights, const glm::vec2& far_left, float step);
private:
    std::vector<std::shared_ptr<Biome>> _active_biomes;

};