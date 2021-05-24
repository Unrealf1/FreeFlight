#pragma once

#include "render/DrawableUnit.hpp"
#include "player/PlayerDependable.hpp"
#include "render/RenderDefs.hpp"
#include "terrain/TerrainChunk.hpp"

#include <deque>


class Terrain: public DrawableUnit, public PlayerDependable  {
    using chunkContainer_t = std::deque<TerrainChunk>;

public:
    Terrain(std::size_t chunk_size);
    virtual ~Terrain() = default;

    void draw(const RenderInfo&) override;
    void init() override;
    void playerUpdate(const PlayerInfo&) override;

private:
    GraphicObject _graphics;
    chunkContainer_t _chunks;
    const std::size_t _chunk_limit = instance_render_limit;

    const std::size_t _chunk_size;
    const float _chunk_length = static_cast<float>(_chunk_size);
};
