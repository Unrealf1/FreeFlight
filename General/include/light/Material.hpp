#pragma once

#include <glm/glm.hpp>


struct MaterialInfo {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess = 100.0f;
};
