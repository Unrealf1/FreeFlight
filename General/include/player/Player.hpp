#pragma once

#include "player/PlayerInfo.hpp"
#include "utility/ObjectMover.hpp"

class Player  {
public:
    const PlayerInfo& getInfo() {
        return _info;
    }

    void update(GLFWwindow* window, double dt) {
        mover.update(window, dt);
        _info.pos = mover.getPosition();
        _info.cameraMover->update(window, dt);
    }

    void setViewDistance(float view_distance) {
        _info.view_distance = view_distance;
    }

private:
    PlayerInfo _info;

    FreeObjectMover mover;
};