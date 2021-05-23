#pragma once

#include "render/DrawableUnit.hpp"
#include "player/PlayerDependable.hpp"
#include "camera/Camera.hpp"

#include <memory>
#include <vector>


class Application {
public:
    Application() {

    }

    void start() {

    }

    

private:
    virtual void initGUI() {
        ImGui_ImplGlfwGL3_Init(_window, false);
    }

    virtual void updateGUI() {
        ImGui_ImplGlfwGL3_NewFrame();
    }

    std::vector<std::shared_ptr<DrawableUnit>> _drawable;
    std::vector<std::shared_ptr<PlayerDependable>> _player_dependable;

    PlayerInfo _player;

    std::unique_ptr<CameraMover> _cameraMover;

};

