#include "biomes/BiomeManager.hpp"
#include "biomes/TestBiome.hpp"
#include "biomes/Hills.hpp"
#include "biomes/Field.hpp"
#include "biomes/Desert.hpp"

#include <spdlog/spdlog.h>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <unordered_set>


BiomeManager::BiomeManager() {
    _active_biomes.push_back(std::make_shared<Hills>());
    _active_biomes.push_back(std::make_shared<Field>());
    _active_biomes.push_back(std::make_shared<Desert>());

    _biome_centers.push_back(generateRandomBiomeAt({0.0f, 0.0f}));
}

BiomeCenter BiomeManager::generateRandomBiomeAt(const glm::vec2& position) {
    int32_t new_biome_ix = _random_generator.randomInt(0, _active_biomes.size());
    auto new_biome_id = _active_biomes[new_biome_ix]->getId();
    return {position, new_biome_id};
}

BiomeCenter BiomeManager::findClosestCenter(const glm::vec2& position) {
    auto it = std::ranges::min_element(_biome_centers, [&](const BiomeCenter& c1, const BiomeCenter& c2){ 
        return glm::distance(position, c1.position) < glm::distance(position, c2.position); 
    });
    return *it;
}

void BiomeManager::updateCenters(const glm::vec2& position) {
    auto closest_center = findClosestCenter(position);

    if (glm::distance(position, closest_center.position) < _new_biome_threshold) {
        return;
    }

    auto direction = glm::normalize(position - closest_center.position);
    float range = _random_generator.randomFloat(_min_range, _max_range);

    glm::vec2 new_position = closest_center.position + direction * range;
    _biome_centers.push_back(generateRandomBiomeAt(new_position));
}

std::vector<std::vector<BiomeCenter>> BiomeManager::findAllRelatedCenters(uint32_t points_in_chunk, const glm::vec2& near_left, float step) {
    //float chunk_len = step * static_cast<float>(points_in_chunk);
    std::vector<std::vector<BiomeCenter>> related_centers;
    related_centers.resize(points_in_chunk);

    auto current_position = near_left;
    for (auto& row : related_centers) {
        row.resize(points_in_chunk);
        for (auto& point : row) {
            point = findClosestCenter(current_position);
            current_position.x += step;
        }
        current_position.x = near_left.x;
        current_position.y += step;
    }

    return related_centers;
}


std::shared_ptr<Biome> BiomeManager::getBiomeById(const biomeId_t& id) {
    for (auto& b : _active_biomes) {
        if (b->getId() == id) {
            return b;
        }
    }
    spdlog::error("Unknown biome was asked, returning test biome");
    return std::make_shared<TestBiome>();
}

ChunkVertex BiomeManager::getVertexAt(const glm::vec2& position) {
    auto biome_id = getBiomeIdAt(position);
    auto biome = getBiomeById(biome_id);
    std::vector<std::vector<ChunkVertex>> vertex_container(1);
    vertex_container[0].resize(1);
    biome->generateVertices(vertex_container, position, 0, std::vector<std::vector<BiomeCenter>>(1, {{position, biome_id}}));
    return vertex_container[0][0];
}

// Height before biome merging
float BiomeManager::getRawHeightAt(const glm::vec2& position) {
    return getVertexAt(position).height;
}

biomeId_t BiomeManager::getBiomeIdAt(const glm::vec2& position) {
    return findClosestCenter(position).biome_id;
}

TerrainChunk BiomeManager::generateChunk(uint32_t points_in_chunk, const glm::vec2& near_left, float step) {
    float chunk_len = static_cast<float>(points_in_chunk) * step;
    updateCenters(near_left + glm::vec2{0.0f, chunk_len});
    updateCenters(near_left + glm::vec2{chunk_len, 0.0f});
    updateCenters(near_left + glm::vec2(chunk_len));
    updateCenters(near_left);

    // create and initialize new chunk
    TerrainChunk result;
    result._vertices.resize(points_in_chunk);
    for (auto& row : result._vertices) {
        row.resize(points_in_chunk);
    }

    // give every biome a chance to participate
    auto related_centers = findAllRelatedCenters(points_in_chunk, near_left, step);
    for (auto& biome : _active_biomes) {
        biome->generateVertices(result._vertices, near_left, step, related_centers);
    }

    // make biome borders a bit smoother

    // for (uint32_t i = 0; i < result._vertices.size(); ++i) {
    //     for (uint32_t j = 0; j < result._vertices[i].size(); ++j) {
    //         ChunkVertex& vertex = result._vertices[i][j];
    //         glm::vec2 current_position = glm::vec2(i * step, j * step) + near_left;
    //         const auto& current_biome_id = related_centers[i][j].biome_id;
    //
    //         auto process = [&](glm::vec2 diff) {
    //             auto other1_pos = current_position + diff;
    //             auto other1_biome = getBiomeIdAt(other1_pos);
    //
    //             auto other2_pos = current_position - diff;
    //             auto other2_biome = getBiomeIdAt(other2_pos);
    //
    //             if (other2_biome == other1_biome) {
    //                 return;
    //             }
    //
    //             auto other1 = getVertexAt(other1_pos);
    //             auto other2 = getVertexAt(other2_pos);
    //
    //             vertex.height = (other1.height + other2.height) / 2.0f;
    //             vertex.texture_handler = other1.texture_handler;
    //             vertex.secondary_texture_handler = other2.texture_handler;
    //             vertex.secondary_texture_weight = 0.5f;
    //         };
    //        
    //         process(glm::vec2{0.0f, step});
    //         process(glm::vec2{0.0f, -step});
    //         process(glm::vec2{step, 0.0f});
    //         process(glm::vec2{-step, 0.0f});
    //     }
    // }
    //
    // for (uint32_t i = 1; i < result._vertices.size(); ++i) {
    //     for (uint32_t j = 1; j < result._vertices.size(); ++j) {
    //         ChunkVertex& vertex = result._vertices[i][j];
    //         const auto& current_biome_id = related_centers[i][j].biome_id;
    //
    //         if (related_centers[i-1][j].biome_id != current_biome_id) {
    //             vertex.secondary_texture_handler = result._vertices[i-1][j].texture_handler;
    //             vertex.secondary_texture_weight = 0.8f;
    //             // vertex.height = (vertex.height + result._vertices[i-1][j].height) / 2.0f;
    //         } else if(related_centers[i][j-1].biome_id != current_biome_id) {
    //             vertex.secondary_texture_handler = result._vertices[i][j-1].texture_handler;
    //             vertex.secondary_texture_weight = 0.8f;
    //             // vertex.height = (vertex.height + result._vertices[i][j-1].height) / 2.0f;
    //         } else {
    //             vertex.secondary_texture_handler = vertex.texture_handler;
    //             vertex.secondary_texture_weight = 0.0f;
    //         }
    //     }
    // }
    // for (uint32_t j = 1; j < result._vertices.size(); ++j) {
    //     auto process = [&](uint32_t ii, uint32_t jj, const glm::vec2& diff){
    //         glm::vec2 current_position = glm::vec2(ii * step, jj * step) + near_left;
    //
    //         auto other_pos = current_position + diff;
    //         auto other_biome = getBiomeIdAt(other_pos);
    //         spdlog::debug("biome at {}/{} is {}", other_pos.x, other_pos.y, getBiomeById(other_biome)->name());
    //
    //         auto& this_vertex = result._vertices[ii][jj];
    //
    //         if (other_biome != related_centers[ii][jj].biome_id) {
    //             this_vertex.secondary_texture_weight = 0.8f;
    //             auto other = getVertexAt(other_pos);
    //             this_vertex.secondary_texture_handler = other.texture_handler;
    //         } else {
    //             this_vertex.secondary_texture_weight = 0.0f;
    //             this_vertex.secondary_texture_handler = this_vertex.texture_handler;
    //         }
    //     };
    //
    //     // process(0, j, glm::vec2(0.0f, -step));
    //     process(j, 0, glm::vec2(-step, 0.0f));
    // }


    return result;
}


