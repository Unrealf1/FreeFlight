#pragma once

#include <string>
#include <spdlog/spdlog.h>


struct ApplicationParameters {
    float view_distance;
    float near_plane;
    float far_plane;

    std::string log_location;
    spdlog::level::level_enum log_level;

    std::string window_name;

    int points_in_chunk;
    float chunk_length;
    int64_t max_fps;

    float daylength;
    float dawn_intensity;
};

class ApplicationParametersReader {
public:
    static ApplicationParameters read(const std::string& filepath);
};

class ApplicationParametersWriter {
public:
    static void write(const ApplicationParameters& parameters, const std::string& filepath);
};