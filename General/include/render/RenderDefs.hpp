#pragma once

#include <cstddef>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

constexpr std::size_t instance_render_limit = 100;

template<typename Point = glm::vec3>
using Mesh = std::vector<Point>;

template<typename Point = glm::vec3>
struct Model {
    Mesh<Point> mesh;
    Mesh<Point> norms;
};

template<typename TexPoint = glm::vec2, typename Point = glm::vec3>
struct TexturedModel: Model<Point>{
    Mesh<TexPoint> texCoords;
};


struct GraphicObject {
    GLuint vbo_coords = 0;
    GLuint vbo_norms = 0;
    GLuint vbo_texcoords = 0;
    GLuint vao = 0;
    GLsizei vertex_cnt = 0;
    GLuint texture = 0;
};

