#include "terrain/Terrain.hpp"
#include "render/Drawer.hpp"
#include "render/ProgramContainer.hpp"
#include "render/Program.hpp"
#include "render/RenderDefs.hpp"
#include "render/GraphicsInitializer.hpp"
#include "render/Program.hpp"
#include "utility/ThreadPool.hpp"
#include "render/TextureContainer.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <spdlog/spdlog.h>


static const char* const terrain_program_name = "terrain_program";

void Terrain::draw(const RenderInfo& info) {
    DrawInfo drawInfo;
    Drawer::prepareDraw(drawInfo);

    std::vector<ChunkData> datum;
    datum.reserve(_active_chunks.size());
    auto base_mat = glm::mat4(1.0f);
    std::ranges::transform( 
        _active_chunks, 
        std::back_inserter(datum), 
        [&base_mat, this](const TerrainChunk& c){
            ChunkData result;
            result.model_mat = glm::translate(base_mat, glm::vec3(c._center_location, 0.0f)) * _scale_mat;
            result.heights_buffer_offset = c._ssbo_offset;
            return result;
        }
    );

    auto program = ProgramContainer::getProgram(terrain_program_name);

    glUseProgram(program);

    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, _graphics.texture);

    glBindVertexArray(_graphics.vao);
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

    glUniform3fv(
        glGetUniformLocation(program, "light.dir"),
        1,
        //glm::value_ptr(info.sun->direction)
        glm::value_ptr(glm::vec3(info.view_mat * glm::vec4(info.sun->direction, 0.0)))
    );
    glUniform3fv(
        glGetUniformLocation(program, "light.La"),
        1,
        glm::value_ptr(info.sun->ambient)
    );
    glUniform3fv(
        glGetUniformLocation(program, "light.Ld"),
        1,
        glm::value_ptr(info.sun->diffuse)
    );
    glUniform3fv(
        glGetUniformLocation(program, "light.Ls"),
        1,
        glm::value_ptr(info.sun->specular)
    );

    glUniform1ui(
        glGetUniformLocation(program, "chunk_size"),
        static_cast<GLuint>(_points_in_chunk)
    );

    glUniform1f(
        glGetUniformLocation(program, "step_len"),
        _chunk_length / static_cast<float>(_points_in_chunk)
    );

    std::size_t already_rendered = 0;

    while (already_rendered < datum.size()) {
        const auto num_chunks = std::min(instance_render_limit, datum.size() - already_rendered); 

        for(size_t i = 0; i < num_chunks; i++) {
            std::string index = std::to_string(i);
            size_t datum_ix = i + already_rendered;
            //TODO: rewrite this without loop
            glUniformMatrix4fv(
                glGetUniformLocation(program, ("models[" + index + "]").c_str()),
                1,
                GL_FALSE,
                glm::value_ptr(datum[datum_ix].model_mat)
            );
            glUniformMatrix3fv(
                glGetUniformLocation(program, ("normalToCamera[" + index + "]").c_str()),
                1,
                GL_FALSE,
                //glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(info.view_mat * datum[datum_ix].model_mat))))
                glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(info.view_mat))))
            );
            glUniform1ui(
                glGetUniformLocation(program, ("offsets[" + index + "]").c_str()),
                datum[datum_ix].heights_buffer_offset
            );
        }
        already_rendered += num_chunks;
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _additional_vertex_data_ssbo);

        glDrawArraysInstanced(GL_TRIANGLES, 0, _graphics.vertex_cnt, static_cast<GLsizei>(num_chunks));
    }

    glBindVertexArray(0);
    
}

TexturedModel<> Terrain::createChunkModel() {
    float num_steps = static_cast<float>(_points_in_chunk - 1); // -1 for one less edge than vertex
    float step = 1.0f / num_steps;
    spdlog::info("step is {}", step);

    Mesh<> mesh;
    mesh.reserve(_points_in_chunk * 2);

    Mesh<glm::vec2> chunk_tex_coords;
    chunk_tex_coords.reserve(_points_in_chunk * 2);

    float cur_row = -0.5f;
    float tex_row = 0.0f;
    float tex_repeat_per_chunk = _chunk_length / 20.0f;
    float tex_step = step * tex_repeat_per_chunk;
    for (size_t i = 0; i < _points_in_chunk - 1; ++i) { // from far to near
        float cur_column = -0.5f;
        float tex_column = 0.0f;
        for (size_t j = 0; j < _points_in_chunk - 1; ++j) { // from left to right
            float near_left_ind = static_cast<float>(i * _points_in_chunk + j);
            float near_right_ind = static_cast<float>(i * _points_in_chunk + j + 1);
            float far_left_ind = static_cast<float>((i+1) * _points_in_chunk + j);
            float far_right_ind = static_cast<float>((i+1) * _points_in_chunk + j + 1);

            // upper triangle
            mesh.push_back(glm::vec3(cur_column, cur_row, far_left_ind));
            mesh.push_back(glm::vec3(cur_column, cur_row - step, near_left_ind));
            mesh.push_back(glm::vec3(cur_column + step, cur_row, far_right_ind));

            chunk_tex_coords.push_back(glm::vec2(tex_column, tex_row));
            chunk_tex_coords.push_back(glm::vec2(tex_column, tex_row - tex_step));
            chunk_tex_coords.push_back(glm::vec2(tex_column + tex_step, tex_row));

            //lower triangle
            mesh.push_back(glm::vec3(cur_column + step, cur_row, far_right_ind));
            mesh.push_back(glm::vec3(cur_column, cur_row - step, near_left_ind));
            mesh.push_back(glm::vec3(cur_column + step, cur_row - step, near_right_ind));

            chunk_tex_coords.push_back(glm::vec2(tex_column + tex_step, tex_row));
            chunk_tex_coords.push_back(glm::vec2(tex_column, tex_row - tex_step));
            chunk_tex_coords.push_back(glm::vec2(tex_column + tex_step, tex_row - tex_step));

            cur_column += step;
            tex_column += tex_step;
        }
        tex_row += tex_step;
        cur_row += step;
    }
    spdlog::info("mesh size is {}", mesh.size());
    Mesh<> norms(mesh.size(), {0.0f, 0.0f, 1.0f});

    return {mesh, norms, chunk_tex_coords};
}

void Terrain::init() {
    spdlog::debug("terrain initializing...");

    auto chunk_base_model = createChunkModel();

    // TOOD: textured initializer is called to init texcoords
    _graphics = GraphicsInitializer::initObject(chunk_base_model, "resources/textures/non_existent_texture.jpg");

    spdlog::info("graphics has {} vertices", _graphics.vertex_cnt);

    if (ProgramContainer::checkProgram(terrain_program_name)) {
        return;
    }

    auto text = extractShaderText("resources/shaders/Bindless.vert");
    auto vertex_shader = createVertexShader(text.c_str());

    text = extractShaderText("resources/shaders/Bindless.frag");
    auto frag_shader = createFragmentShader(text.c_str());

    auto prog = createProgram(vertex_shader, frag_shader);

    ProgramContainer::registerProgram(terrain_program_name, prog);

    _additional_vertex_data = new additional_vertex_data[ssbo_buffer_size];
    glGenBuffers(1, &_additional_vertex_data_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _additional_vertex_data_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ssbo_buffer_size * sizeof(additional_vertex_data), _additional_vertex_data, GL_DYNAMIC_DRAW/*GLenum usage*/);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

Terrain::chunkIt_t Terrain::findChunkCloseTo(
    const glm::vec2& position, 
    chunkContainer_t& container
) const {
    return std::find_if(
        container.begin(), 
        container.end(), 
        [&position, this](const TerrainChunk& c) { 
            return glm::abs(c._center_location.x - position.x) <= _chunk_length/2.0f && glm::abs(c._center_location.y - position.y) <= _chunk_length/2.0f;
        }
    );
}

float Terrain::get_closest_chunk_center(float a) {
    auto sign = a > 0.0f ? 1.0f : -1.0f;
    a = a < 0.0f ? -a : a;
    if (a <= _chunk_length / 2.0f) {
        return 0.0f;
    }
    //TODO: optimize
    for (float center = _chunk_length; ; center += _chunk_length) {
        if (a < center) {
            float closest = (center - a) < (a + _chunk_length - center) ? center : (center - _chunk_length); 
            return closest * sign;
        }
    }
}

glm::vec2 Terrain::get_closest_possible_chunk_center(const glm::vec2& position) {
    return {get_closest_chunk_center(position.x), get_closest_chunk_center(position.y)};
}

TerrainChunk Terrain::generateChunkAt(const glm::vec2& position) {
    if (_points_in_chunk > chunk_size_limit) {
        spdlog::critical("too much points in one chunk");
        exit(1);
    }

    auto near_left = position - glm::vec2(_chunk_length/2.0f);
    float num_steps = static_cast<float>(_points_in_chunk - 1);
    float step = 1.0f / num_steps * _chunk_length;
    TerrainChunk result = _biomeManager->generateChunk(_points_in_chunk, near_left, step);
    result._center_location = position;

    auto regenerate_biome_information = [&](Terrain::chunkIt_t iter){
        if (iter == _active_chunks.end()) {
            return;
        }
        // TODO: why is this necessary?
        auto tmp = _biomeManager->generateChunk(_points_in_chunk, iter->_center_location - glm::vec2(_chunk_length/2.0f), step);
        iter->_vertices = tmp._vertices;
    };
    // this chunks should be updated as biome manager might changed their biome distribution
    auto left_chunk = findChunkCloseTo(result._center_location - glm::vec2(_chunk_length, 0.0f), _active_chunks);
    auto near_chunk = findChunkCloseTo(result._center_location - glm::vec2(0.0f, _chunk_length), _active_chunks);
    auto right_chunk = findChunkCloseTo(result._center_location + glm::vec2(_chunk_length, 0.0f), _active_chunks);
    auto far_chunk = findChunkCloseTo(result._center_location + glm::vec2(0.0f, _chunk_length), _active_chunks);
    regenerate_biome_information(left_chunk);
    regenerate_biome_information(near_chunk);
    regenerate_biome_information(right_chunk);
    regenerate_biome_information(far_chunk);

    //std::ranges::for_each(_active_chunks, regenerate_biome_information);

    _active_chunks_updated = true;

    return result;

}

Terrain::constChunkIt_t Terrain::getChunkCloseTo(const glm::vec2& position) {
    auto chunk_pos = get_closest_possible_chunk_center(position);
    auto found = findChunkCloseTo(chunk_pos, _active_chunks);
    if (found != _active_chunks.end()) {
        return found;
    }
    spdlog::debug("no active chunk at position {}/{}", position.x, position.y);

    found = findChunkCloseTo(chunk_pos, _archived_chunks);
    if (found != _archived_chunks.end()) {
        spdlog::info("the chunk was archived, making it active again");
        _active_chunks.push_back(*found);
        _active_chunks_updated = true;
        _archived_chunks.erase(found);
        return found;
    }
    //spdlog::info("chunk is not archived...");
    spdlog::debug("Generating new chunk at {} {}", chunk_pos.x, chunk_pos.y);
    auto new_chunk = generateChunkAt(chunk_pos);
    _active_chunks.push_back(new_chunk);
    _active_chunks_updated = true;

    return (--_active_chunks.end());
}

void Terrain::check_chunks_containers() {
    //spdlog::debug("active chunks: {}. archived chunks: {}", _active_chunks.size(), _archived_chunks.size());
    while (_active_chunks.size() > _active_chunk_limit) {
        _active_chunks.pop_front();
        _active_chunks_updated = true;
    }

    while (_archived_chunks.size() > _archived_chunks_limit) {
        _archived_chunks.pop_front();
    }
}

void Terrain::playerUpdate(const PlayerInfo& player) {
    glm::vec2 top_left{player.pos.x - _chunk_length, player.pos.y + _chunk_length};

    chunkContainer_t temp_chunks;

    auto chunk_window = std::llround(_view_distance / _chunk_length);
    auto chunk_window_len = static_cast<float>(chunk_window);

    for (int64_t i = 0; i < chunk_window * 2 + 1; ++i) {
        float cur_x = player.pos.x + _chunk_length * chunk_window_len - _chunk_length * float(i);
        for (int64_t j = 0; j < chunk_window * 2 + 1; ++j) {
            float cur_y = player.pos.y + _chunk_length * chunk_window_len - _chunk_length * float(j);
            (void)getChunkCloseTo({cur_x, cur_y});
        }
    }

    check_chunks_containers();
    if (_active_chunks_updated) {
        updateBuffers();
        flushBuffers();
        _active_chunks_updated = false;
    }
}

void Terrain::updateBuffers() {
    const auto chunk_data_size = _points_in_chunk * _points_in_chunk;
    
    for (size_t c_i = 0; c_i < _active_chunks.size(); ++c_i) {
        auto& chunk = _active_chunks[c_i];
        auto chunk_offset = static_cast<uint32_t>(c_i) * chunk_data_size;
        chunk._ssbo_offset = chunk_offset;
        for (uint32_t i = 0; i < chunk._vertices.size(); ++i) {
            for (uint32_t j = 0; j < chunk._vertices[i].size(); ++j) {
                uint32_t index = chunk_offset + i * _points_in_chunk + j;
                _additional_vertex_data[index].height = chunk._vertices[i][j].height;
                _additional_vertex_data[index].texture = chunk._vertices[i][j].texture_handler;
                _additional_vertex_data[index].secondary_texture = chunk._vertices[i][j].secondary_texture_handler;
                _additional_vertex_data[index].secondary_texture_weight = chunk._vertices[i][j].secondary_texture_weight;
                
                if (j > 0) { // if there's vertex to the left in this chunk
                    auto left_neighboor_index = index - 1;
                    // this vertex is it's right neighboor
                    _additional_vertex_data[left_neighboor_index].indexes.right = index; 
                }

                if (j < chunk._vertices[i].size() - 1) { // if there's vertex to the right in this chunk
                    auto right_neighboor_index = index + 1;
                    _additional_vertex_data[right_neighboor_index].indexes.left = index;
                }

                if (i > 0) {
                    auto near_neighboor_index = index - _points_in_chunk;
                    _additional_vertex_data[near_neighboor_index].indexes.far = index;
                }
                if (i < chunk._vertices.size() - 1) {
                    auto far_neighboor_index = index + _points_in_chunk;
                    _additional_vertex_data[far_neighboor_index].indexes.near = index;
                }
            }
        }
    }
    
    // another pass, when all chunks are guaranteed to have vailid chunk_offset calculated
    for (size_t c_i = 0; c_i < _active_chunks.size(); ++c_i) {
        auto& chunk = _active_chunks[c_i];
        auto left_chunk = findChunkCloseTo(chunk._center_location - glm::vec2(_chunk_length, 0.0f), _active_chunks);
        auto near_chunk = findChunkCloseTo(chunk._center_location - glm::vec2(0.0f, _chunk_length), _active_chunks);
        auto right_chunk = findChunkCloseTo(chunk._center_location + glm::vec2(_chunk_length, 0.0f), _active_chunks);
        auto far_chunk = findChunkCloseTo(chunk._center_location + glm::vec2(0.0f, _chunk_length), _active_chunks);

        size_t i = 0; // near end of the chunk
        if (near_chunk != _active_chunks.end()) {
            for (size_t j = 0; j < chunk._vertices[i].size(); ++j) {
                uint32_t index = chunk._ssbo_offset + i * _points_in_chunk + j;
                auto near_neighboor_index = near_chunk->_ssbo_offset + (near_chunk->_vertices.size() - 1) * _points_in_chunk + j;
                _additional_vertex_data[near_neighboor_index].indexes.far = index;
            }
        } else {
            for (size_t j = 0; j < chunk._vertices[i].size(); ++j) {
                uint32_t index = chunk._ssbo_offset + i * _points_in_chunk + j;
                _additional_vertex_data[index].indexes.near = index;
            }
        }


        i = chunk._vertices.size() - 1; // far end of the chunk
        if (far_chunk != _active_chunks.end()) {
            for (size_t j = 0; j < chunk._vertices[i].size(); ++j) {
                uint32_t index = chunk._ssbo_offset + i * _points_in_chunk + j;
                auto far_neighboor_index = far_chunk->_ssbo_offset + 0 * _points_in_chunk + j;
                _additional_vertex_data[far_neighboor_index].indexes.near = index;
            }
        } else {
            for (size_t j = 0; j < chunk._vertices[i].size(); ++j) {
                uint32_t index = chunk._ssbo_offset + i * _points_in_chunk + j;
                _additional_vertex_data[index].indexes.far = index;
            }
        }

        i = 0; // left end of the chunk
        if (left_chunk != _active_chunks.end()) {
            for (size_t j = 0; j < chunk._vertices.size(); ++j) {
                uint32_t index = chunk._ssbo_offset + j * _points_in_chunk + i;
                auto left_neighboor_index = left_chunk->_ssbo_offset + j * _points_in_chunk + (_points_in_chunk - 1);
                _additional_vertex_data[left_neighboor_index].indexes.right = index;
            }
        } else {
            for (size_t j = 0; j < chunk._vertices.size(); ++j) {
                uint32_t index = chunk._ssbo_offset + j * _points_in_chunk + i;
                _additional_vertex_data[index].indexes.left = index;
            }
        }

        i = chunk._vertices.size() - 1; // right end of the chunk
        if (right_chunk != _active_chunks.end()) {
            for (size_t j = 0; j < chunk._vertices.size(); ++j) {
                uint32_t index = chunk._ssbo_offset + j * _points_in_chunk + i;
                auto right_neighboor_index = right_chunk->_ssbo_offset + j * _points_in_chunk + 0;
                _additional_vertex_data[right_neighboor_index].indexes.left = index;
            }
        } else {
            for (size_t j = 0; j < chunk._vertices.size(); ++j) {
                uint32_t index = chunk._ssbo_offset + j * _points_in_chunk + i;
                _additional_vertex_data[index].indexes.right = index;
            }
        }
    }
}
    
void Terrain::flushBuffers() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _additional_vertex_data_ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, ssbo_buffer_size * sizeof(additional_vertex_data), _additional_vertex_data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

std::string Terrain::getBiomeNameAt(const glm::vec2& coords) {
    auto id = _biomeManager->getBiomeIdAt(coords);
    return _biomeManager->getBiomeById(id)->name();
}

float Terrain::getHeightAt(const glm::vec2& coords) {
    // this is not ideal: smoothing is not respected
    // and displayed height is less precise, as it is
    // interpolated between points
    return _biomeManager->getRawHeightAt(coords);

    //TODO: improve height calculation and interpolate between points
    auto find_height_in_chunk = [this](TerrainChunk& c, const glm::vec2& pos) {
        const float half_len = _chunk_length / 2.0f;
        glm::vec2 near_left = c._center_location - glm::vec2(half_len, half_len);
        glm::vec2 offsets = pos - near_left;
        const float step = _chunk_length / static_cast<float>(c._vertices.size() - 1);
        float rounded_offset_x = std::roundf(offsets.x / step);
        float rounded_offset_y = std::roundf(offsets.y / step);
        
        glm::vec2 closest_point = glm::vec2{rounded_offset_x, rounded_offset_y} * step + near_left;

        auto x_i = static_cast<uint64_t>(rounded_offset_x);
        auto y_i = static_cast<uint64_t>(rounded_offset_y);

        x_i = std::min(std::max(x_i, 0ul), c._vertices.size() - 1ul);
        y_i = std::min(std::max(y_i, 0ul), c._vertices.size() - 1ul);

        auto diff = closest_point - pos; 
        const float dir_x = diff.x < 0.0f ? -1.0f : 1.0f;
        const float dir_y = diff.y < 0.0f ? -1.0f : 1.0f;
        glm::vec2 p1 = closest_point;
        glm::vec2 p2 = closest_point + glm::vec2(step * dir_x, 0.0f);
        glm::vec2 p3 = closest_point + glm::vec2(0.0f, step * dir_y);

        float h1 = c._vertices[y_i][x_i].height;

        size_t x2_i = x_i;
        if (x_i > 0 && dir_x == -1.0f) {
            x2_i -= 1;
        } 
        if (x_i < c._vertices.size() - 1ul && dir_x == 1.0f) {
            x2_i += 1;
        }

        float h2 = c._vertices[y_i][x2_i].height;

        size_t y2_i = y_i;
        if (y_i > 0 && dir_y == -1.0f) {
            y2_i -= 1;
        } 
        if (y_i < c._vertices.size() - 1ul && dir_y == 1.0f) {
            y2_i += 1;
        }

        float h3 = c._vertices[y2_i][x_i].height;
        
        // в данный момент есть три точки, задающие плоскость в которой мы ищем высоту
        // значит зная уровнение плоскости, по X, Y можно вычислить Z.

        glm::vec3 d21 = glm::vec3(p2.x, p2.y, h2) - glm::vec3(p1.x, p1.y, h1);
        glm::vec3 d31 = glm::vec3(p3.x, p3.y, h3) - glm::vec3(p1.x, p1.y, h1);

        const float s1 = d21.y*d31.z - d21.z*d31.y;
        const float s2 = d21.x*d31.z - d21.z*d31.x;
        const float s3 = d21.x*d31.y - d21.y*d31.x;

        const float A = s1;
        const float B = s2;
        const float C = s3;
        const float D = -p1.x * s1 + p1.y * s2 - h1*s3;

        float res_h = -(A*pos.x + B*pos.y + D) / C;
        // spdlog::debug(
        //    "POINTS:\n1: {}\t/\t{}\t({})\n2: {}\t/\t{}\t({})\n3: {}\t/\t{}\t({})\n", 
        //    closest_point.x, closest_point.y, h1, p2.x, p2.y, h2, p3.x, p3.y, h3
        // );
        // spdlog::debug(
        //    "s1/s2/s3:\n1: {}\t/\t{}\t({})\n", 
        //     s1, s2, s3
        // );
        // spdlog::debug(
        //    "A: {} B:{} C:{} D:{}", 
        //     A, B, C, D
        // );
        return h1;
        return res_h;

        //spdlog::debug("\nDIST:\n1: {}\t2: {}\t3: {}\n1: {}\t2: {}\t3: {}\n", dist1, dist2, dist3, k1, k2, k3);
        

        //return c._vertices[y_i][x_i].height;
    };

    auto chunk = findChunkCloseTo(coords, _active_chunks);
    if (chunk != _active_chunks.end()) {
        return find_height_in_chunk(*chunk, coords);
    }

    chunk = findChunkCloseTo(coords, _archived_chunks);

    if (chunk != _archived_chunks.end()) {
        return find_height_in_chunk(*chunk, coords);
    }
    spdlog::warn("height of unloaded chunk was requested. Returning zero");
    return 0.0f;
}
