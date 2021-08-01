#pragma once

#include "biomes/Biome.hpp"
#include "random/Generator.hpp"

#include <vector>
#include <memory>
#include <glm/glm.hpp>


class BiomeManager {
    struct MergeRequest {
        BiomeCenter center;
        TerrainChunk::vertexMap_t vertices;
    };

public:
    BiomeManager();
    ~BiomeManager() = default;

    std::shared_ptr<Biome> get_biome_at(const glm::vec3& position);
    TerrainChunk generateChunk(uint32_t points_in_chunk, const glm::vec2& near_left, float step);
    float getRawHeightAt(const glm::vec2& position);
    biomeId_t getBiomeIdAt(const glm::vec2& position);
    std::shared_ptr<Biome> getBiomeById(const biomeId_t& id);
private:
    std::vector<std::shared_ptr<Biome>> _active_biomes;
    std::vector<BiomeCenter> _biome_centers;

    Generator _random_generator;

    const float _new_biome_threshold = 2000.0f;
    const float _min_range = 1600.0f;
    const float _max_range = 2200.0f;

    BiomeCenter findClosestCenter(const glm::vec2& position);

    std::vector<std::vector<BiomeCenter>> findAllRelatedCenters(uint32_t points_in_chunk, const glm::vec2& near_left, float step);
    BiomeCenter generateRandomBiomeAt(const glm::vec2& position);
    void updateCenters(const glm::vec2& position);
    ChunkVertex getVertexAt(const glm::vec2& position);
};