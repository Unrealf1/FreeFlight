#pragma once

#include <unordered_map>
#include <string>
#include <GL/glew.h>

class ProgramContainer {
public:
    static bool checkProgram(const std::string& name) {
        return _programs.find(name) != _programs.end();
    }

    static void registerProgram(const std::string& name, GLuint id) {
        _programs.insert(std::make_pair(name, id));
    }

    static GLuint getProgram(const std::string& name) {
        return _programs.at(name);
    }

private:
    static std::unordered_map<std::string, GLuint> _programs;
};