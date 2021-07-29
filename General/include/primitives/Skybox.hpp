#pragma once

#include "render/DrawableUnit.hpp"
#include "render/TextureContainer.hpp"
#include "render/RenderInfo.hpp"
#include "render/Updatable.hpp"

#include <array>
#include <string>
#include <spdlog/spdlog.h>
#include <SOIL2.h>
#include <stb_image.h>


static const char* const skybox_program_name = "skybox_prog";

class Skybox : public DrawableUnit, public Updatable {
public:
    Skybox() = default;

    void update(const UpdateInfo& info) {
        if (info.daytime < 0.5f) {
            // on 0.0 -> 0.5
            // on 0.5 -> 0.0
            _day_night_ratio = 0.5f - info.daytime;
        } else if (info.daytime < 1.5f) {
            // 0.5 -> 0.0
            // 1.5 -> 1.0
            _day_night_ratio = info.daytime - 0.5f;
        } else {
            // 1.5 -> 1.0
            // 2.0 -> 0.5
            _day_night_ratio = 2.5f - info.daytime;
        }
    }

    void draw(const RenderInfo& info) {
        glDepthFunc(GL_LEQUAL);

        glm::mat4 view = glm::mat4(glm::mat3(info.view_mat));
		const glm::mat4& projection = info.proj_mat;

        auto program = ProgramContainer::getProgram(skybox_program_name);
        glUseProgram(program);

        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        glUniform1f(
            glGetUniformLocation(program, "night_weight"),
            _day_night_ratio
        );
        spdlog::info("_day_night_ratio is {}", _day_night_ratio);

		// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
		// where an object is present (a depth of 1.0f will always fail against any object's depth value)
		glBindVertexArray(_vao);

        GLint tex1_loc = glGetUniformLocation( program, "skybox_day" );
        GLint tex2_loc = glGetUniformLocation( program, "skybox_night" );

		//glActiveTexture(GL_TEXTURE0);
		///glBindTexture(GL_TEXTURE_CUBE_MAP, _day_texture);
        //glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, _night_texture);

        glUniform1i( tex1_loc, 0 );
        glUniform1i( tex2_loc, 1 );

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

        glDepthFunc(GL_LESS);
    }


    void init() {
        unsigned int skyboxVAO, skyboxVBO, skyboxEBO;

        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ebo);

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        std::array<std::string, 6> names = {"right", "left", "front", "back", "bottom", "top"};
        std::string extension = ".png";

        auto init_cube_texture = [&](std::string folder_name, uint32_t tex_unit) {
            GLuint tex;

            glGenTextures(1, &tex);
            glActiveTexture(tex_unit);
            glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            // These are very important to prevent seams
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

            for (uint32_t i = 0; i < names.size(); i++) {
                int width, height, nrChannels;
                auto filepath = tex_folder + folder_name + names[i] + extension;
                unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
                auto format = nrChannels == 4 ? GL_RGBA : GL_RGB;
                if (data) {
                    stbi_set_flip_vertically_on_load(false);
                    glTexImage2D (
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                        0,
                        format,
                        width,
                        height,
                        0,
                        format,
                        GL_UNSIGNED_BYTE,
                        data
                    );
                    spdlog::debug("Loaded skybox texture \"{}\"", filepath);
                }
                else {
                    spdlog::error("Failed to load skybox texture \"{}\"", filepath);
                }
                stbi_image_free(data);
            }
            return tex;
        };

        _day_texture = init_cube_texture("day/", GL_TEXTURE0);
        _night_texture = init_cube_texture("night/", GL_TEXTURE1);

        auto text = extractShaderText("resources/shaders/Skybox.vert");
        auto vertex_shader = createVertexShader(text.c_str());

        text = extractShaderText("resources/shaders/Skybox.frag");
        auto frag_shader = createFragmentShader(text.c_str());

        auto prog = createProgram(vertex_shader, frag_shader);

        ProgramContainer::registerProgram(skybox_program_name, prog);

    }    

private:
    GLuint _vao;
    GLuint _vbo;
    GLuint _ebo;
    GLuint _day_texture;
    GLuint _night_texture;
    std::string tex_folder = "resources/textures/skybox/";
    float _day_night_ratio = 0.0f;

    float skyboxVertices[24] = {
        //   Coordinates
        -1.0f, -1.0f,  1.0f,//        7--------6
         1.0f, -1.0f,  1.0f,//       /|       /|
         1.0f, -1.0f, -1.0f,//      4--------5 |
        -1.0f, -1.0f, -1.0f,//      | |      | |
        -1.0f,  1.0f,  1.0f,//      | 3------|-2
         1.0f,  1.0f,  1.0f,//      |/       |/
         1.0f,  1.0f, -1.0f,//      0--------1
        -1.0f,  1.0f, -1.0f
    };

    unsigned int skyboxIndices[36] = {
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3
    };
};
