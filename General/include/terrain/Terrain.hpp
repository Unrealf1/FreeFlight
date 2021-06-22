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
    struct ChunkData {
        GLuint heights_buffer_offset;
        glm::mat4 model_mat;
    };

public:
    explicit Terrain(uint32_t points_in_chunk, float chunk_scale, float view_distance)
    : _points_in_chunk(points_in_chunk), _chunk_length(chunk_scale), _view_distance(view_distance) {}
    virtual ~Terrain() = default;

    void draw(const RenderInfo&) override;
    void init() override;
    void playerUpdate(const PlayerInfo&) override;

    // const means, that this will __not__ create new chunks.
    float getHeightAt(const glm::vec2& coords) const;

    using chunkContainer_t = std::deque<TerrainChunk>;
    using chunkIt_t = chunkContainer_t::iterator;
    using constChunkIt_t = chunkContainer_t::const_iterator;

private:
    void initGraphics();

    GraphicObject _graphics;
    chunkContainer_t _archived_chunks;
    chunkContainer_t _active_chunks;
    static constexpr std::size_t _active_chunk_limit = instance_render_limit;
    const std::size_t _archived_chunks_limit = 1000;
    static constexpr std::size_t chunk_size_limit = 2000;
    static constexpr std::size_t max_points_in_chunk = chunk_size_limit * chunk_size_limit;
    static constexpr std::size_t heights_buffer_size = max_points_in_chunk * _active_chunk_limit;

    // number of vertices in one side of a chunk
    const uint32_t _points_in_chunk;
    GLuint _heights_ssbo;
    float* _heights;
    
    // actual length of the chunk
    const float _chunk_length;
    glm::mat4 _scale_mat = glm::scale(glm::mat4(1.0f), glm::vec3(_chunk_length, _chunk_length, 1.0f));
    
    float _view_distance;

    std::unique_ptr<BiomeManager> _biomeManager = std::make_unique<BiomeManager>();

    constChunkIt_t findChunkCloseTo(const glm::vec2& position, const chunkContainer_t& container) const;
    float get_closest_chunk_center(float a);
    glm::vec2 get_closest_possible_chunk_center(const glm::vec2& position);
    TerrainChunk generateChunkAt(const glm::vec2& position);
    constChunkIt_t getChunkCloseTo(const glm::vec2& position);
    void check_chunks_containers();

    TexturedModel<> createChunkModel();

    void updateHeights();

};
