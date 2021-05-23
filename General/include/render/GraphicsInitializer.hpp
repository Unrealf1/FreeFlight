#pragma once

#include "render/RenderDefs.hpp"

#include <string>

class GraphicsInitializer {
public:
    static GraphicObject initObject(const Model&);

    static GraphicObject initObject(const TexturedModel&, const std::string& texture_file_path);

    static GLuint initTexture(const std::string& texture_file_path);
};
