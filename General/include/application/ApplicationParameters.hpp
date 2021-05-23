#pragma once

#include <string>

struct ApplicationParameters {
    float view_distance;
};

class ApplicationParametersReader {
public:
    static ApplicationParameters read(const std::string& filepath);
};

class ApplicationParametersWriter {
public:
    static void write(const ApplicationParameters& parameters, const std::string& filepath);
};