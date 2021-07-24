#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "light/Light.hpp"

struct RenderInfo {
    glm::mat4 view_mat;
    glm::mat4 proj_mat;
    std::shared_ptr<DirectionalLight> sun;
};
