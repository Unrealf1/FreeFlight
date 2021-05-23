#include "application/ApplicationParameters.hpp"

#include <json/json.h>
#include <fstream>

static const char* const view_distance_id = "view_distance";

ApplicationParameters ApplicationParametersReader::read(const std::string& filepath) {
    Json::Value root;
    auto file = std::ifstream(filepath);
    file >> root;
    ApplicationParameters result;
    result.view_distance = root[view_distance_id].asFloat();

    return result;
}

void ApplicationParametersWriter::write(const ApplicationParameters& parameters, const std::string& filepath) {
    Json::Value json;
    json[view_distance_id] = parameters.view_distance;

    auto file = std::ofstream(filepath);
    file << json;
}