#pragma once

#include <unordered_map>
#include <string>
#include <GL/glew.h>

#include "render/GraphicsInitializer.hpp"


class TextureContainer {
public:
    static bool checkTexture(const std::string& name) {
        return _textures.find(name) != _textures.end();
    }

    inline static const std::string debugTextureName = "resources/textures/test1.jpg";

    static GLuint getTexture(const std::string& name) {
        if (checkTexture(name)) {
            return _textures.at(name);
        }

        _textures[name] = GraphicsInitializer::initTexture(name);
        return _textures.at(name);
    }

private:
    static std::unordered_map<std::string, GLuint> _textures;
};


class TexHandleContainer {
public:
    static bool checkHandle(const std::string& name) {
        return _handles.find(name) != _handles.end();
    }

    static void registerHandle(const std::string& name, GLuint64 handle) {
        _handles.insert(std::make_pair(name, handle));
    }

    static GLuint64 createHandle(GLuint texture, GLuint sampler) {
        return glGetTextureSamplerHandleARB(texture, sampler);
        
    }

    static GLuint64 getHandle(const std::string& name) {
        return _handles.at(name);
    }

private:
    static std::unordered_map<std::string, GLuint64> _handles;
};