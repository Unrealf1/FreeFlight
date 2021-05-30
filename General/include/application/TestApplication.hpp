#pragma once

#include "application/Application.hpp"
#include "render/DrawableUnit.hpp"
#include "camera/Camera.hpp"
#include "primitives/Square.hpp"


class TestApplication: public Application {
public:
    TestApplication(const ApplicationParameters& params): Application(params) { }

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
    std::shared_ptr<CameraMover> _cameraMover;

    std::vector<std::shared_ptr<DrawableUnit>> _drawable;
    std::vector<std::shared_ptr<Updatable>> _updatable;

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
            ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

            if (ImGui::Button("Button")) {
                _logger->info("button was pressed");                
            }
        }
        ImGui::End();
    }

    virtual void prepareScene() override {
        _cameraMover = std::make_unique<OrbitCameraMover>();
        //_cameraMover = std::make_unique<FreeCameraMover>(5.0f);
        _cameraMover.get()->setNearFarPlanes(_params.near_plane, _params.far_plane);
        _cameraMover.get()->setNearFarPlanes(0.1f, 2000.0f);

        _drawable.push_back(std::make_shared<Square>(10));

        for (auto& item : _drawable) {
            item->init();
        }
    }

    virtual void onUpdate(const UpdateInfo& info) override {
        _cameraMover->update(info.window, info.dt);

        for (auto& item : _updatable) {
            item->update(info);
        }
    }

    virtual void onRun() override {
        _logger->debug("Test application is running");
    }

    virtual void onStop() override {
        _logger->debug("Test application is stopping");
    }


};