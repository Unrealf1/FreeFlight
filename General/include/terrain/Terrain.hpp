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
    : _points_in_chunk(points_in_chunk), ssbo_buffer_size(_active_chunk_limit * _points_in_chunk * _points_in_chunk), _chunk_length(chunk_scale), _view_distance(view_distance) {}
    virtual ~Terrain() {
        if (_additional_vertex_data != nullptr) {
            delete[] _additional_vertex_data;
        }
    }

    void draw(const RenderInfo&) override;
    void init() override;
    void playerUpdate(const PlayerInfo&) override;

    // const means, that this will __not__ create new chunks.
    float getHeightAt(const glm::vec2& coords);

    using chunkContainer_t = std::deque<TerrainChunk>;
    using chunkIt_t = chunkContainer_t::iterator;
    using constChunkIt_t = chunkContainer_t::const_iterator;

private:
    struct neightboors_indexes {
        uint32_t left;
        uint32_t far;
        uint32_t right;
        uint32_t near;
    };

    struct additional_vertex_data {
        float height;
        GLuint64 texture;
        GLuint64 secondary_texture;
        float secondary_texture_weight;
        neightboors_indexes indexes;
    };

    void initGraphics();

    // number of vertices in one side of a chunk
    const uint32_t _points_in_chunk;

    GraphicObject _graphics;
    chunkContainer_t _archived_chunks;
    chunkContainer_t _active_chunks;
    static constexpr std::size_t _active_chunk_limit = instance_render_limit;
    const std::size_t _archived_chunks_limit = 1000;
    static constexpr std::size_t chunk_size_limit = 500;
    static constexpr std::size_t max_points_in_chunk = chunk_size_limit * chunk_size_limit;
    const std::size_t ssbo_buffer_size;
    
    GLuint _additional_vertex_data_ssbo;
    additional_vertex_data* _additional_vertex_data = nullptr;

    bool _active_chunks_updated = false; 
    
    // actual length of the chunk
    const float _chunk_length;
    glm::mat4 _scale_mat = glm::scale(glm::mat4(1.0f), glm::vec3(_chunk_length, _chunk_length, 1.0f));
    
    float _view_distance;

    std::unique_ptr<BiomeManager> _biomeManager = std::make_unique<BiomeManager>();

    chunkIt_t findChunkCloseTo(const glm::vec2& position, chunkContainer_t& container) const;
    float get_closest_chunk_center(float a);
    glm::vec2 get_closest_possible_chunk_center(const glm::vec2& position);
    TerrainChunk generateChunkAt(const glm::vec2& position);
    constChunkIt_t getChunkCloseTo(const glm::vec2& position);
    void check_chunks_containers();

    TexturedModel<> createChunkModel();

    void updateBuffers();
    void flushBuffers();

};
