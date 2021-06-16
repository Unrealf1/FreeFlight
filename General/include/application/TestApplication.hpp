#pragma once

#include "application/Application.hpp"
#include "render/DrawableUnit.hpp"
#include "camera/Camera.hpp"
#include "primitives/Square.hpp"
#include "player/PlayerController.hpp"
#include "terrain/Terrain.hpp"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;


class TestApplication: public Application {
    using frame_clock = std::chrono::steady_clock;

public:
    TestApplication(const ApplicationParameters& params): Application(params), 
    _max_fps(params.max_fps), _min_duration(frame_clock::duration(1s) / _max_fps) { }

    virtual void handleKey(int key, int scancode, int action, int mods) override {
        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_ESCAPE)
            {
                glfwSetWindowShouldClose(_window, GL_TRUE);
            }
        }

        _cameraMover->handleKey(_window, key, scancode, action, mods);
    }

    virtual void handleMouseMove(double xpos, double ypos) override {
        if (/*ImGui::IsMouseHoveringAnyWindow()*/ false)
        {
            return;
        }

        _cameraMover->handleMouseMove(_window, xpos, ypos);
    }

    virtual void handleScroll(double xoffset, double yoffset) override {
        _cameraMover->handleScroll(_window, xoffset, yoffset);
    }
protected:
    std::shared_ptr<FreeCameraMover> _cameraMover;

    std::vector<std::shared_ptr<DrawableUnit>> _drawable;
    std::vector<std::shared_ptr<Updatable>> _updatable;
    std::vector<std::shared_ptr<PlayerDependable>> _player_dependable;

    frame_clock::time_point _last_frame_time;
    int64_t _fps = 0; // current
    int64_t _max_fps;
    frame_clock::duration _min_duration;

    virtual void draw() override {
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto cam_info = _cameraMover->cameraInfo();
        RenderInfo renderInfo;
        renderInfo.view_mat = cam_info.viewMatrix;
        renderInfo.proj_mat = cam_info.projMatrix;
        //...
        for (auto& item : _drawable) {
            item->draw(renderInfo);
        }
    }

    virtual void updateGUI() override {
        Application::updateGUI();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("GUI", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("FPS %.1f", static_cast<double>(ImGui::GetIO().Framerate));
            //ImGui::Text("FPS: %i", _fps);
            auto pos = _cameraMover->getPos();
            ImGui::Text("Coordinates(x, y, z): %.1f, %.1f, %.1f", pos.x, pos.y, pos.z);

            if (ImGui::Button("Speed+")) {
                auto old_spd = _cameraMover->getSpeed();
                _cameraMover->setSpeed(old_spd * 2);
            }
            if (ImGui::Button("Speed-")) {
                auto old_spd = _cameraMover->getSpeed();
                _cameraMover->setSpeed(old_spd / 2);
            }
        }
        ImGui::End();
    }

    virtual void prepareScene() override {
        _logger->error("duration in milliseconds: {}", std::chrono::duration_cast<std::chrono::milliseconds>(_min_duration).count());
        //_cameraMover = std::make_unique<OrbitCameraMover>();
        _cameraMover = std::make_unique<FreeCameraMover>(20.0f);
        _cameraMover.get()->setNearFarPlanes(_params.near_plane, _params.far_plane);

        _drawable.push_back(std::make_shared<Square>(4.0f));

        _logger->debug("creating terrain...");
        auto terrain = std::make_shared<Terrain>(_params.points_in_chunk, _params.chunk_length, _params.view_distance);
        _drawable.push_back(terrain);
        _player_dependable.push_back(terrain);

        for (auto& item : _drawable) {
            item->init();
        }
    }

    virtual void onUpdate(const UpdateInfo& info) override {
        _cameraMover->update(info.window, info.dt);

        for (auto& item : _updatable) {
            item->update(info);
        }

        PlayerInfo playerInfo;
        playerInfo.pos = _cameraMover->getPos();

        for (auto& item : _player_dependable) {
            item->playerUpdate(playerInfo);
        }

        auto cur_time = frame_clock::now();
        auto duration = cur_time - _last_frame_time;
        _last_frame_time = cur_time;
        _fps = 1s / duration * 0.6 + 0.4 * _fps;

        std::this_thread::sleep_for(_min_duration - duration);
    }

    virtual void onRun() override {
        _logger->debug("Test application is running");
    }

    virtual void onStop() override {
        _logger->debug("Test application is stopping");
    }


};