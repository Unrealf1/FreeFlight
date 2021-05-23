#include "application/ApplicationParameters.hpp"

#include <json/json.h>
#include <fstream>

ApplicationParameters ApplicationParametersReader::read(const std::string& filepath) {
    Json::Value root;
    auto file = std::ifstream(filepath);
    file >> root;
}

void ApplicationParametersWriter::write(const ApplicationParameters& parameters, const std::string& filepath) {

}