#pragma once

#include "render/RenderDefs.hpp"

#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include <SOIL2.h>


class GraphicsInitializer {
public:
    template<typename Point>
    static std::vector<float> flattenMesh(const Mesh<Point>& mesh) {
        std::vector<float> flattened;
        flattened.reserve(mesh.size() * Point::length());
        for (auto& point : mesh) {
            for (glm::vec3::length_type i = 0; i < Point::length(); ++i)
            flattened.push_back(point[i]);
        }
        return flattened;
    }

    template<typename Point> 
    static GraphicObject initObject(const Model<Point>& model) {
        GraphicObject _go;
        auto total_points = model.mesh.size();
        auto coords = flattenMesh(model.mesh);
        auto norms = flattenMesh(model.norms);
        _go.vertex_cnt = static_cast<GLsizei>(total_points);
        // create vao
        glGenVertexArrays(1, &_go.vao);
        glBindVertexArray(_go.vao);
        // create vbo for coords
        glGenBuffers(1, &_go.vbo_coords);
        glBindBuffer(GL_ARRAY_BUFFER, _go.vbo_coords);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(coords.size() * sizeof(float)), 
            coords.data(),
            GL_STATIC_DRAW);
        // enable vbo for coords
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        // create vbo for norms
        glGenBuffers(1, &_go.vbo_norms);
        glBindBuffer(GL_ARRAY_BUFFER, _go.vbo_norms);
        glBufferData(
            GL_ARRAY_BUFFER, 
            static_cast<GLsizeiptr>(norms.size() * sizeof(float)),
            norms.data(),
            GL_STATIC_DRAW
        );
        // enable vbo for norms
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        return _go;
    }

    template<typename Point>
    static GraphicObject initObject(const TexturedModel<Point>& model, const std::string& texture_file_path) {
        GraphicObject go = initObject(model);
        
        go.texture = initTexture(texture_file_path);

        auto texture_coords = flattenMesh(model.texCoords);
        
        glGenBuffers(1, &go.vbo_texcoords);
        glBindBuffer(GL_ARRAY_BUFFER, go.vbo_texcoords);
        glBufferData(
            GL_ARRAY_BUFFER, 
            static_cast<GLsizeiptr>(texture_coords.size() * sizeof(float)), 
            texture_coords.data(), 
            GL_STATIC_DRAW
        );
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

        return go;
    }

    static GLuint initTexture(const std::string& texture_file_path) {
        GLuint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        int width, height;
        unsigned char* image = SOIL_load_image(texture_file_path.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
        if (!image) {
            spdlog::error("SOIL loading error for texture at \"{}\": {}", texture_file_path, SOIL_last_result());
            return 0;
        }
        spdlog::debug("From file {}, created texture with id {}, w={}, h={}", texture_file_path, texture, width, height);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;
    }
};
