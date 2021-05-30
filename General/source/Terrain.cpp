#include "terrain/Terrain.hpp"
#include "render/Drawer.hpp"
#include "render/ProgramContainer.hpp"
#include "render/RenderDefs.hpp"
#include "render/GraphicsInitializer.hpp"
#include "render/Program.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <spdlog/spdlog.h>



static const char* const terrain_program_name = "terrain_program";

void Terrain::draw(const RenderInfo& info) {
    DrawInfo drawInfo;
    Drawer::prepareDraw(drawInfo);

    std::vector<glm::mat4> matrices;
    matrices.reserve(_active_chunks.size());
    auto base_mat = glm::mat4(1.0f);
    std::transform(
        _active_chunks.begin(), 
        _active_chunks.end(), 
        std::back_inserter(matrices), [&base_mat](const TerrainChunk& c){ 
            return glm::translate(base_mat, glm::vec3(c._center_location, 0.0f));
    });

    auto program = ProgramContainer::getProgram(terrain_program_name);

    glUseProgram(program);

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

    const auto num_chunks = std::min(instance_render_limit, matrices.size()); 
    for(size_t i = 0; i < num_chunks; i++) {
        std::string index = std::to_string(i);
        glUniformMatrix4fv(
            glGetUniformLocation(program, ("models[" + index + "]").c_str()),
            1,
            GL_FALSE,
            glm::value_ptr(matrices[i])
        );

        glDrawArraysInstanced(GL_TRIANGLES, 0, _graphics.vertex_cnt, num_chunks);
    } 
}

void Terrain::init() {
    _graphics = GraphicsInitializer::initObject(_chunk_base_model);

    if (ProgramContainer::checkProgram(terrain_program_name)) {
        return;
    }

    auto text = extractShaderText("shaders/Basic.vert");
    auto vertex_shader = createVertexShader(text.c_str());

    text = extractShaderText("shaders/Basic.frag");
    auto frag_shader = createFragmentShader(text.c_str());

    auto prog = createProgram(vertex_shader, frag_shader);

    ProgramContainer::registerProgram(terrain_program_name, prog);
}

Terrain::constChunkIt_t Terrain::findChunkCloseTo(
    const glm::vec2& position, 
    chunkContainer_t& container
) {
    return std::find_if(
        container.begin(), 
        container.end(), 
        [&position, this](const TerrainChunk& c) { 
            return glm::length(c._center_location - position) < _chunk_size;
        }
    );
}

float Terrain::get_closest_chunk_center(float a) {
    int sign = a > 0 ? 1 : -1;
    a = a < 0.0f ? -a : a;
    if (a <= _chunk_half_length) {
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
    heightmap.reserve(_chunk_size);
    for (auto& row : heightmap) {
        row.reserve(_chunk_size);
    }

    auto low_left = position - glm::vec2(_chunk_length, _chunk_length);
    spdlog::info("requesting heights from biome manager");
    spdlog::info("biomeManager address is {}", (void*)_biomeManager.get());
    _biomeManager->generateHeights(heightmap, low_left);
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
    
    return (--_active_chunks.end());
}

void Terrain::check_chunks_containers() {
    spdlog::debug("active chunks: {}. archived chunks: {}", _active_chunks.size(), _archived_chunks.size());
    while (_active_chunks.size() > _active_chunk_limit) {
        _active_chunks.pop_front();
    }

    while (_archived_chunks.size() > _archived_chunks_limit) {
        _archived_chunks.pop_front();
    }
}

void Terrain::playerUpdate(const PlayerInfo& player) {
    glm::vec2 top_left{player.pos.x - _chunk_size, player.pos.y + _chunk_size};

    chunkContainer_t temp_chunks;

    std::size_t chunk_window = player.view_distance / _chunk_size;

    for (size_t i = 0; i < chunk_window * 2 + 1; ++i) {
        float cur_x = player.pos.x + float(_chunk_length * chunk_window) - float(_chunk_length * i);
        for (size_t j = 0; j < chunk_window * 2 + 1; ++j) {
            float cur_y = player.pos.y + float(_chunk_length * chunk_window) - float(_chunk_length * j);
            (void)getChunkCloseTo({cur_x, cur_y});
        }
    }
    check_chunks_containers();
}
