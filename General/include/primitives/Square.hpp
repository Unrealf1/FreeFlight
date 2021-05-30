#pragma once

#include "render/DrawableUnit.hpp"
#include "render/RenderDefs.hpp"
#include "render/Program.hpp"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>


class Square : public DrawableUnit {
public:
    Square(float scale = 1.0f) {
        _modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
    }

    virtual void draw(const RenderInfo& info) {
        glUseProgram(_program);
        glBindVertexArray(_go.vao);
        glUniformMatrix4fv(
            glGetUniformLocation(_program, "view"),
            1,
            GL_FALSE,
            glm::value_ptr(info.view_mat)
        );
        glUniformMatrix4fv(
            glGetUniformLocation(_program, "projection"),
            1,
            GL_FALSE,
            glm::value_ptr(info.proj_mat)
        );
        glUniformMatrix4fv(
            glGetUniformLocation(_program, "model"),
            1,
            GL_FALSE,
            glm::value_ptr(_modelMat)
        );
        glDrawArrays(GL_TRIANGLES, 0, _go.vertex_cnt);
        glBindVertexArray(0);
    }

    virtual void init() {
        spdlog::debug("Square initializing...");
        _go.vertex_cnt = coords.size() / 3;
        // create vao
        glGenVertexArrays(1, &_go.vao);
        glBindVertexArray(_go.vao);
        // create vbo for coords
        glGenBuffers(1, &_go.vbo_coords);
        glBindBuffer(GL_ARRAY_BUFFER, _go.vbo_coords);
        glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(float), coords.data(), GL_STATIC_DRAW);
        // enable vbo for coords
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        auto text = extractShaderText("shaders/Basic.vert");
        auto vertex_shader = createVertexShader(text.c_str());

        text = extractShaderText("shaders/Basic.frag");
        auto frag_shader = createFragmentShader(text.c_str());

        _program = createProgram(vertex_shader, frag_shader);
    }

private:
    GLuint _program;
    GraphicObject _go;
    glm::mat4 _modelMat;

    const float side_size = 2.0f;

    std::vector<float> coords = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, side_size,
        side_size, 0.0f, side_size,
        side_size, 0.0f, side_size,
        side_size, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };
};