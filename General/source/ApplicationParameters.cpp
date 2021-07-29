#include "application/ApplicationParameters.hpp"

#include <json/json.h>
#include <fstream>
#include <unordered_map>


static const char* const view_distance_id = "view_distance";
static const char* const near_plane_id = "near_plane";
static const char* const far_plane_id = "far_plane";
static const char* const log_location_id = "log_location";
static const char* const log_level_id = "log_level";
static const char* const window_name_id = "window_name";
static const char* const points_in_chunk_id = "points_in_chunk";
static const char* const chunk_length_id = "chunk_length";
static const char* const max_fps_id = "max_fps";
static const char* const daylength_id = "daylength";
static const char* const dawn_intensity_id = "dawn_intensity";


ApplicationParameters ApplicationParametersReader::read(const std::string& filepath) {
    Json::Value root;
    auto file = std::ifstream(filepath);
    file >> root;
    ApplicationParameters result;

    auto extract_float = [&](const char* const id) {
        return root[id].asFloat();
    };

    result.view_distance = extract_float(view_distance_id);
    result.far_plane = extract_float(far_plane_id);
    result.near_plane = extract_float(near_plane_id);
    result.chunk_length = extract_float(chunk_length_id);
    result.log_level = ([&](){
        auto raw = root[log_level_id].asInt();
        if (raw == 1) {
            return spdlog::level::err;
        } else if (raw == 2) {
            return spdlog::level::debug;
        } else {
            return spdlog::level::off;
        }
    })();
    result.log_location = root[log_location_id].asString();
    result.window_name = root[window_name_id].asString();
    result.points_in_chunk = root[points_in_chunk_id].asInt();
    result.max_fps = root[max_fps_id].asInt();
    result.daylength = extract_float(daylength_id);
    result.dawn_intensity = extract_float(dawn_intensity_id);

    return result;
}

void ApplicationParametersWriter::write(const ApplicationParameters& parameters, const std::string& filepath) {
    Json::Value json;
    json[view_distance_id] = parameters.view_distance;
    json[far_plane_id] = parameters.far_plane;
    json[near_plane_id] = parameters.near_plane;
    json[log_level_id] = ([&](){
        auto& val = parameters.log_level;
        if (val == spdlog::level::off) {
            return 0;
        } if (val == spdlog::level::err) {
            return 1;
        } if (val == spdlog::level::debug) {
            return 2;
        } else {
            return -1;
        }
    })();
    json[log_location_id] = parameters.log_location;
    json[window_name_id] = parameters.window_name;
    json[points_in_chunk_id] = parameters.points_in_chunk;
    json[chunk_length_id] = parameters.chunk_length;
    json[max_fps_id] = parameters.max_fps;
    json[daylength_id] = parameters.daylength;
    json[dawn_intensity_id] = parameters.dawn_intensity;

    auto file = std::ofstream(filepath);
    file << json;
}