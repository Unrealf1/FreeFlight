#include "terrain/Terrain.hpp"
#include "render/Drawer.hpp"
#include "render/ProgramContainer.hpp"
#include "render/Program.hpp"
#include "render/RenderDefs.hpp"
#include "render/GraphicsInitializer.hpp"
#include "render/Program.hpp"

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
    std::transform(
        _active_chunks.begin(), 
        _active_chunks.end(), 
        std::back_inserter(datum), [&base_mat, this](const TerrainChunk& c){
            ChunkData result;
            result.model_mat = glm::translate(base_mat, glm::vec3(c._center_location, 0.0f)) * _scale_mat;
            result.heights_buffer_offset = c._heights_buffer_offset;
            return result;
    });

    auto program = ProgramContainer::getProgram(terrain_program_name);

    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _graphics.texture);

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

    glUniform1ui(
        glGetUniformLocation(program, "chunk_size"),
        static_cast<GLuint>(_points_in_chunk)
    );

    // GLuint block_index = 0;
    // block_index = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "terrain_heights");
    // //spdlog::info("block index is {}", block_index);
    // GLuint ssbo_binding_point_index = 1;
    // glShaderStorageBlockBinding(program, block_index, ssbo_binding_point_index);

    const auto num_chunks = std::min(instance_render_limit, datum.size()); 
    for(size_t i = 0; i < num_chunks; i++) {
        std::string index = std::to_string(i);
        glUniformMatrix4fv(
            glGetUniformLocation(program, ("models[" + index + "]").c_str()),
            1,
            GL_FALSE,
            glm::value_ptr(datum[i].model_mat)
        );
        glUniform1ui(
            glGetUniformLocation(program, ("offsets[" + index + "]").c_str()),
            datum[i].heights_buffer_offset
        );
        
    } 
    glDrawArraysInstanced(GL_TRIANGLES, 0, _graphics.vertex_cnt, static_cast<GLsizei>(num_chunks));
    glBindVertexArray(0);
    
}

TexturedModel<> Terrain::createChunkModel() {
    float num_steps = static_cast<float>(_points_in_chunk - 1);
    float step = 1.0f / num_steps;
    spdlog::info("step is {}", step);

    Mesh<> mesh;
    mesh.reserve(_points_in_chunk * 2);

    Mesh<glm::vec2> chunk_tex_coords;
    chunk_tex_coords.reserve(_points_in_chunk * 2);

    float cur_row = 0.5f;
    float tex_row = 1.0f;
    float tex_repeat_per_chunk = _chunk_length / 10.0f;
    float tex_step = step * tex_repeat_per_chunk;
    for (size_t i = 0; i < _points_in_chunk - 1; ++i) { // from far to near
        float cur_column = -0.5f;
        float tex_column = 0.0f;
        for (size_t j = 0; j < _points_in_chunk - 1; ++j) { // from left to right
            float far_left_ind = static_cast<float>(i * _points_in_chunk + j);
            float far_right_ind = static_cast<float>(i * _points_in_chunk + j + 1);
            float near_left_ind = static_cast<float>((i+1) * _points_in_chunk + j);
            float near_right_ind = static_cast<float>((i+1) * _points_in_chunk + j + 1);

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
        tex_row -= tex_step;
        cur_row -= step;
    }
    spdlog::info("mesh size is {}", mesh.size());
    Mesh<> norms(mesh.size(), {0.0f, 0.0f, 1.0f});

    return {mesh, norms, chunk_tex_coords};
}

void Terrain::init() {
    spdlog::debug("terrain initializing...");

    auto chunk_base_model = createChunkModel();

    _graphics = GraphicsInitializer::initObject(chunk_base_model, "resources/textures/grass3.jpg");

    spdlog::info("graphics has {} vertices", _graphics.vertex_cnt);

    if (ProgramContainer::checkProgram(terrain_program_name)) {
        return;
    }

    auto text = extractShaderText("resources/shaders/Terrain.vert");
    auto vertex_shader = createVertexShader(text.c_str());

    text = extractShaderText("resources/shaders/Terrain.frag");
    auto frag_shader = createFragmentShader(text.c_str());

    auto prog = createProgram(vertex_shader, frag_shader);

    ProgramContainer::registerProgram(terrain_program_name, prog);

    _heights = new float[heights_buffer_size];
    glGenBuffers(1, &_heights_ssbo);
    spdlog::info("ssbo is {}", _heights_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _heights_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, heights_buffer_size * sizeof(float), _heights, GL_DYNAMIC_DRAW/*GLenum usage*/);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _heights_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

Terrain::constChunkIt_t Terrain::findChunkCloseTo(
    const glm::vec2& position, 
    const chunkContainer_t& container
) const {
    return std::find_if(
        container.cbegin(), 
        container.cend(), 
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
    TerrainChunk result;
    result._center_location = position;
    std::vector<std::vector<float>> heightmap;
    if (_points_in_chunk > chunk_size_limit) {
        spdlog::critical("too much points in one chunk");
        exit(1);
    }
    heightmap.resize(_points_in_chunk);
    for (auto& row : heightmap) {
        row.resize(_points_in_chunk);
    }

    auto far_left = position + glm::vec2(-_chunk_length/2.0f, _chunk_length/2.0f);
    spdlog::info("requesting heights from biome manager");
    float num_steps = static_cast<float>(_points_in_chunk - 1);
    float step = 1.0f / num_steps;
    _biomeManager->generateHeights(heightmap, far_left, step * _chunk_length);
    spdlog::info("transforming heightmap into chunk");

    std::transform(
        heightmap.begin(), 
        heightmap.end(), 
        std::back_inserter(result._vertices), 
        [](const std::vector<float>& vf) {
            std::vector<ChunkVertex> res;
            res.reserve(vf.size());

            std::transform(
                vf.begin(), 
                vf.end(), 
                std::back_inserter(res), 
                [](float f){ return ChunkVertex{f}; }
            );

            return res;
        }
    );

    return result;

}

Terrain::constChunkIt_t Terrain::getChunkCloseTo(const glm::vec2& position) {
    // spdlog::info("searching for the first chunk");
    auto chunk_pos = get_closest_possible_chunk_center(position);
    auto found = findChunkCloseTo(chunk_pos, _active_chunks);
    if (found != _active_chunks.end()) {
        // spdlog::info("the chunk is active already!");
        return found;
    }
    spdlog::info("chunk is not active...");

    found = findChunkCloseTo(chunk_pos, _archived_chunks);
    if (found != _archived_chunks.end()) {
        spdlog::info("the chunk was archived");
        _active_chunks.push_back(*found);
        _archived_chunks.erase(found);
        return found;
    }
    spdlog::info("chunk is not archived...");
    spdlog::debug("Generating new chunk at {} {}", chunk_pos.x, chunk_pos.y);
    auto new_chunk = generateChunkAt(chunk_pos);
    _active_chunks.push_back(new_chunk);
    updateHeights();
    
    return (--_active_chunks.end());
}

void Terrain::check_chunks_containers() {
    //spdlog::debug("active chunks: {}. archived chunks: {}", _active_chunks.size(), _archived_chunks.size());
    while (_active_chunks.size() > _active_chunk_limit) {
        _active_chunks.pop_front();
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
}

void Terrain::updateHeights() {
    //loading heights data to the gpu
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _heights_ssbo);
    const auto chunk_data_size = _points_in_chunk * _points_in_chunk;

    for (size_t c_i = 0; c_i < _active_chunks.size(); ++c_i) {
        auto& chunk = _active_chunks[c_i];
        auto chunk_offset = static_cast<uint32_t>(c_i) * chunk_data_size;
        for (size_t i = 0; i < chunk._vertices.size(); ++i) {
            for (size_t j = 0; j < chunk._vertices[i].size(); ++j) {
                _heights[chunk_offset + i * _points_in_chunk + j] = chunk._vertices[i][j].height;
                chunk._heights_buffer_offset = chunk_offset;
            }
        }
    }
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, heights_buffer_size, _heights);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

float Terrain::getHeightAt(const glm::vec2& coords) const {
    auto chunk = findChunkCloseTo(coords, _active_chunks);
    spdlog::error("for chunk at pos {}/{} for query at {}/{}", chunk->_center_location.x, chunk->_center_location.y, coords.x, coords.y);
    auto find_height_in_chunk = [this](const TerrainChunk& c, const glm::vec2& pos) {
        float half_len = _chunk_length / 2.0f;
        glm::vec2 near_left = c._center_location - glm::vec2(half_len, half_len);
        glm::vec2 indices = pos - near_left;
        auto x_i = std::llround(indices.x / _chunk_length * static_cast<float>(c._vertices.size()));
        auto y_i = std::llround (indices.y / _chunk_length * static_cast<float>(c._vertices.size())); 
        x_i = std::min(std::max(x_i, 0ll), static_cast<long long>(c._vertices.size() - 1));
        y_i = std::min(std::max(y_i, 0ll), static_cast<long long>(c._vertices.size() - 1));
        y_i = c._vertices.size() - y_i;
        spdlog::error("diff_x is {}, len is {}, ratio is {}", indices.x, _chunk_length, indices.x / _chunk_length);
        spdlog::error("indices: {} {}, while size is {}", x_i, y_i, c._vertices.size());
        spdlog::error("height is {}", c._vertices[x_i][y_i].height);
        auto cc = const_cast<TerrainChunk&>(c);
        cc._vertices[x_i][y_i].height += 1.0f;
        return c._vertices[x_i][y_i].height;
    };

    if (chunk != _active_chunks.end()) {
        return find_height_in_chunk(*chunk, coords);
    }

    chunk = findChunkCloseTo(coords, _archived_chunks);

    if (chunk != _archived_chunks.end()) {
        return find_height_in_chunk(*chunk, coords);
    }

    return 0.0f;
}
