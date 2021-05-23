#pragma once

#include "player/PlayerInfo.hpp"

class PlayerDependable {
public:
    PlayerDependable() = default;
    virtual ~PlayerDependable() = default;

    virtual void playerUpdate(const PlayerInfo&) = 0;
};
