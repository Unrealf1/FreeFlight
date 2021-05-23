#pragma once

#include <vector>

struct ChunkVertex {
    float height;
};

struct TerrainChunk {
    using vertex_t = ChunkVertex; 
    using vertexMap_t = std::vector<std::vector<vertex_t>>;

public:
    vertexMap_t _vertices;

};

