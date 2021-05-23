#pragma once

#include <glm/glm.hpp>
 
using playerPos_t = glm::vec3;

struct PlayerInfo {
    playerPos_t pos;
    float view_distance;
};
