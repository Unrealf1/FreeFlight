#pragma once

#include "render/DrawableUnit.hpp"
#include "render/RenderDefs.hpp"
#include "render/Program.hpp"
#include "render/ProgramContainer.hpp"
#include "render/GraphicsInitializer.hpp"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>


class Square : public DrawableUnit {
public:
    Square(float scale = 1.0f) {
        _modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
    }

    virtual void draw(const RenderInfo& info) {
        auto program = ProgramContainer::getProgram("simple prog");
        glUseProgram(program);
        glBindVertexArray(_go.vao);
        glUniformMatrix4fv(
            glGetUniformLocation(program, "view"),
            1,
            GL_FALSE,
            glm::value_ptr(info.view_mat)
        );
        glUniformMatrix4fv(
            glGetUniformLocation(program, "projection"),
            1,
            GL_FALSE,
            glm::value_ptr(info.proj_mat)
        );
        glUniformMatrix4fv(
            glGetUniformLocation(program, "model"),
            1,
            GL_FALSE,
            glm::value_ptr(_modelMat)
        );
        glDrawArrays(GL_TRIANGLES, 0, _go.vertex_cnt);
        glBindVertexArray(0);
    }

    virtual void init() {
        spdlog::debug("Square: initializing...");
        
        _go = GraphicsInitializer::initObject(_model);

        if (ProgramContainer::checkProgram("simple prog")) {
            return;
        }

        spdlog::debug("Square: creating program ...");

        auto text = extractShaderText("shaders/Basic.vert");
        auto vertex_shader = createVertexShader(text.c_str());

        text = extractShaderText("shaders/Basic.frag");
        auto frag_shader = createFragmentShader(text.c_str());

        auto program = createProgram(vertex_shader, frag_shader);
        ProgramContainer::registerProgram("simple prog", program);
    }

private:
    GraphicObject _go;
    glm::mat4 _modelMat;

    const float side_size = 1.0f;

    Model<glm::vec3> _model{
        // vertices
        {
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, side_size},
            {side_size, 0.0f, side_size},
            {side_size, 0.0f, side_size},
            {side_size, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
        },
        // norms
        {
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
        }
    };
};