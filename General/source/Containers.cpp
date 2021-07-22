#include "render/ProgramContainer.hpp"
#include "render/TextureContainer.hpp"



std::unordered_map<std::string, GLuint> ProgramContainer::_programs;
std::unordered_map<std::string, GLuint> TextureContainer::_textures;
std::unordered_map<std::string, GLuint64> TexHandleContainer::_handles;