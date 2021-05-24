#pragma once

#include "application/Application.hpp"
#include "render/DrawableUnit.hpp"
#include "terrain/Terrain.hpp"

class ConcreteApplication: public Application {
public:
    ConcreteApplication(const ApplicationParameters& params): Application(params) { }

    virtual void handleKey(int key, int scancode, int action, int mods) override {
        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_ESCAPE)
            {
                glfwSetWindowShouldClose(_window, GL_TRUE);
            }
        }

        _player.cameraMover->handleKey(_window, key, scancode, action, mods);
    }

    virtual void handleMouseMove(double xpos, double ypos) override {
        if (/*ImGui::IsMouseHoveringAnyWindow()*/ false)
        {
            return;
        }

        _player.cameraMover->handleMouseMove(_window, xpos, ypos);
    }

    virtual void handleScroll(double xoffset, double yoffset) override {
        _player.cameraMover->handleScroll(_window, xoffset, yoffset);
    }
protected:
    PlayerInfo _player;

    std::vector<std::shared_ptr<DrawableUnit>> _drawable;
    std::vector<std::shared_ptr<PlayerDependable>> _player_dependable;
    std::vector<std::shared_ptr<Updatable>> _updatable;

    virtual void draw() override {
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto cam_info = _player.cameraMover->cameraInfo();
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
            ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

            if (ImGui::Button("Button")) {
                _logger->info("button was pressed");                
            }
        }
        ImGui::End();
    }

    virtual void prepareScene() override {
        _player.cameraMover = std::make_unique<FreeCameraMover>(5.0f);
        _player.cameraMover.get()->setNearFarPlanes(_params.near_plane, _params.far_plane);

        _logger->debug("creating terrain...");
        auto terrain = std::make_shared<Terrain>(10000.0f);
        terrain->init();
        _drawable.push_back(terrain);
        _player_dependable.push_back(terrain);
        _logger->debug("terrain created!");
    }

    virtual void onUpdate(const UpdateInfo& info) override {
        for (auto& item : _updatable) {
            item->update(info);
        }

        for (auto& item : _player_dependable) {
            item->playerUpdate(_player);
        }

        _player.cameraMover->update(info.window, info.dt);
    }

    virtual void onRun() override {
        _logger->debug("onRun");
    }

    virtual void onStop() override {
        _logger->debug("onStop");
    }


};