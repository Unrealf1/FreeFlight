#pragma once

#include <vector>
#include <glm/glm.hpp>

struct ChunkVertex {
    float height;
    
};

struct TerrainChunk {
    using vertex_t = ChunkVertex; 
    using vertexMap_t = std::vector<std::vector<vertex_t>>;

    glm::vec2 _center_location;
    vertexMap_t _vertices;
    uint32_t _heights_buffer_offset;
};

