#pragma once

#include "camera/Camera.hpp"

#include <glm/glm.hpp>
#include <memory>


using playerPos_t = glm::vec3;

struct PlayerInfo {
    playerPos_t pos;
    float view_distance;

    std::unique_ptr<CameraMover> cameraMover;
};
