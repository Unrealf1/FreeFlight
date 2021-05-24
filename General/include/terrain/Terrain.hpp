#pragma once

#include "render/DrawableUnit.hpp"
#include "player/PlayerDependable.hpp"
#include "render/RenderDefs.hpp"
#include "terrain/TerrainChunk.hpp"
#include "biomes/Biome.hpp"
#include "biomes/BiomeManager.hpp"

#include <deque>
#include <vector>


class Terrain: public DrawableUnit, public PlayerDependable  {
public:
    explicit Terrain(std::size_t chunk_size): _chunk_size(chunk_size) {}
    virtual ~Terrain() = default;

    void draw(const RenderInfo&) override;
    void init() override;
    void playerUpdate(const PlayerInfo&) override;

    using chunkContainer_t = std::deque<TerrainChunk>;
    using chunkIt_t = chunkContainer_t::iterator;
    using constChunkIt_t = chunkContainer_t::iterator;

private:
    void initGraphics();

    GraphicObject _graphics;
    chunkContainer_t _archived_chunks;
    chunkContainer_t _active_chunks;
    const std::size_t _active_chunk_limit = instance_render_limit;
    const std::size_t _archived_chunks_limit = 1000;

    const std::size_t _chunk_size;
    
    const float _chunk_length = static_cast<float>(_chunk_size);
    const float _chunk_half_length = _chunk_length / 2.0f;

    Model<> _chunk_base_model = {
        Mesh<> {
            glm::vec3{-_chunk_half_length, -_chunk_half_length, 0.0f}, 
            glm::vec3{_chunk_half_length, -_chunk_half_length, 0.0f},
            glm::vec3{_chunk_half_length, _chunk_half_length, 0.0f},
            glm::vec3{_chunk_half_length, _chunk_half_length, 0.0f},
            glm::vec3{-_chunk_half_length, _chunk_half_length, 0.0f},
            glm::vec3{-_chunk_half_length, -_chunk_half_length, 0.0f}
        },
        Mesh<> {
            glm::vec3{0.0f, 0.0f, 1.0f}, 
            glm::vec3{0.0f, 0.0f, 1.0f}, 
            glm::vec3{0.0f, 0.0f, 1.0f},
            glm::vec3{0.0f, 0.0f, 1.0f}, 
            glm::vec3{0.0f, 0.0f, 1.0f}, 
            glm::vec3{0.0f, 0.0f, 1.0f}
        }
    }; 

    std::unique_ptr<BiomeManager> _biomeManager = std::make_unique<BiomeManager>();

    constChunkIt_t findChunkCloseTo(const glm::vec2& position, chunkContainer_t& container);
    float get_closest_chunk_center(float a);
    glm::vec2 get_closest_possible_chunk_center(const glm::vec2& position);
    TerrainChunk generateChunkAt(const glm::vec2& position);
    constChunkIt_t getChunkCloseTo(const glm::vec2& position);
    void check_chunks_containers();
};
