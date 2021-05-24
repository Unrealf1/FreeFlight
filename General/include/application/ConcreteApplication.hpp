#pragma once

#include "application/Application.hpp"
#include "render/DrawableUnit.hpp"

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


        RenderInfo info;
        //...
        for (auto& item : _drawable) {
            item->draw(info);
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
        } else {
            _logger->error("gui error");
        }
        ImGui::End();
    }

    virtual void prepareScene() override {
        _player.cameraMover = std::make_unique<FreeCameraMover>(5.0f);
        _player.cameraMover.get()->setNearFarPlanes(_params.near_plane, _params.far_plane);
    }

    virtual void onUpdate(const UpdateInfo& info) override {
        for (auto& item : _updatable) {
            item->update(info);
        }
    }

    virtual void onRun() override {
        _logger->debug("onRun");
    }

    virtual void onStop() override {
        _logger->debug("onStop");
    }


};