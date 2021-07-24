#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

struct ChunkVertex {
    float height;
    GLuint64 texture_handler;
    GLuint64 secondary_texture_handler;
    float secondary_texture_weight;
};

struct TerrainChunk {
    using vertex_t = ChunkVertex; 
    using vertexMap_t = std::vector<std::vector<vertex_t>>;

    glm::vec2 _center_location;
    vertexMap_t _vertices;
    uint32_t _ssbo_offset;
};

