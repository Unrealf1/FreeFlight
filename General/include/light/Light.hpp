#pragma once

#include <glm/glm.h>


struct Light {
    glm::vec3 ambient = glm::vec3(0.2, 0.2, 0.2);
    glm::vec3 diffuse = glm::vec3(0.8, 0.8, 0.8);
    glm::vec3 specular = glm::vec3(1.0, 1.0, 1.0);
    float a0;
    float a1;
    float a2;
};

struct DirectionalLight: public Light {
    glm::vec3 direction_to_source;
};

struct PointLight: public Light {
    glm::vec3 position;
};
